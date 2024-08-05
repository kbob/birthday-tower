from collections import namedtuple
from queue import SimpleQueue, Empty
import re
import serial
import threading
import time
import tkinter as tk

# events Teensy -> Tk:
#   connected with params
#   disconnected
# events Tk -> Teensy:
#   set param value
#   quit
# so two queues

teensy_to_tk = SimpleQueue()
tk_to_teensy = SimpleQueue()

class Parameter(namedtuple('Parameter',
        'group_member, min, max, default, units')):
    pass

class ConnectEvent(namedtuple('ConnectEvent', 'params')):
    pass

class DisconnectEvent:
    pass

class QuitEvent:
    pass

class ParamUpdateEvent(namedtuple('ParamUpdateEvent',
        'group_member new_value')):
    pass

float_pattern = r'[-+]? (?: \d+(?:\.\d*)? | \.\d+ ) (?:[Ee][-+]?\d+)?'

param_pattern = rf'''(?x)    # verbose regex syntax
    (?P<group_member> \w+\.\w+) \s+
    (?P<min> {float_pattern}) \s+
    (?P<max> {float_pattern}) \s+
    (?P<default> {float_pattern}) \s+
    (?: (?P<units> \w+) )? \n
'''
param_match = re.compile(param_pattern).match


class TeensyThread(threading.Thread):

    def __init__(self):
        super().__init__()

    def run(self):
        while True:

            # Open the Teensy serial port.
            try:
                port = serial.serial_for_url('hwgrep://16C0&n=2')
            except serial.serialutil.SerialException:
                time.sleep(1)
                continue
            time.sleep(0.2) # Wait for Teensy to finish setup.

            # Collect a list of parameters.
            port.write(b'list\n')
            params = []
            while True:
                line = port.readline()
                if line == b'-\n':
                    break
                param = self.parse_param(line)
                if param:
                    params.append(param)

            # Send the parameters to tkinter.
            teensy_to_tk.put(ConnectEvent(params))

            # Monitor the Teensy status, send parameters as needed.
            while True:
                
                # Get a parameter update.
                try:
                    event = tk_to_teensy.get(block=True, timeout=1)
                    if isinstance(event, QuitEvent):
                        return
                    elif isinstance(event, ParamUpdateEvent):
                        cmd = f'set {event.group_member} {event.new_value}\n'
                        port.write(cmd.encode('utf-8'))
                except Empty:
                    pass

                # Check that Teensy is connected.
                try:
                    port.cts
                except OSError as x:
                    # Teensy died.
                    del port
                    teensy_to_tk.put(DisconnectEvent())
                    time.sleep(5) # let Teensyduino run
                    break
    
    def parse_param(self, param):
        m = param_match(param.decode('utf-8'))
        if m:
            return Parameter(**m.groupdict())


class App(tk.Tk):

    def __init__(self):
        super().__init__()
        self.title('Test')
        self.text = tk.Text(height=10, width=50)
        self.text.grid(row=0, column=0, sticky=tk.NE + tk.SW)
        self.set_text('Hello')
        self.monitor_queue()
        self.teensy_thread = TeensyThread()
        self.teensy_thread.start()
        self.bind('<Destroy>', self.handle_destroy_event)

    def handle_destroy_event(self, event):
        if self.teensy_thread.is_alive():
            tk_to_teensy.put(QuitEvent())
            self.teensy_thread.join()

    def set_text(self, text):
        self.text.delete('1.0', tk.END)
        self.text.insert(tk.END, text)

    def monitor_queue(self):
        self.after(200, self.monitor_queue)
        try:
            event = teensy_to_tk.get_nowait()
        except Empty:
            return
        if isinstance(event, DisconnectEvent):
            self.set_text('Disconnected')
        elif isinstance(event, ConnectEvent):
            self.set_text(self.format_params(event.params))

    def format_params(self, params):
        def format_one(param):
            (gm, n, x, d, u) = param
            u = u or ''
            return f'{gm:20} {n:5} {x:5} {d:5} {u}'
        return '\n'.join(format_one(p) for p in params)


if __name__ == '__main__':
    app = App()
    app.mainloop()

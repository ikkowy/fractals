#!/usr/bin/env python3

import os

from collections import deque

import tornado.ioloop
import tornado.web
import tornado.websocket

import json

#-------------------------------------------------------------------------------

view = None
nodes = set()
nodes_ready = set()
queue = deque()

#-------------------------------------------------------------------------------

def send_info():
    global view
    if view is not None:
        view.write_message(json.dumps(
            {
                'event' : 'info',
                'nodes_count' : len(nodes)
            }
        ))

def send_ready():
    global view
    if view is not None:
        view.write_message(json.dumps(
            { 'event' : 'ready' }
        ))

def send_nudge(node):
    node.write_message(json.dumps(
        { 'event' : 'nudge' }
    ))

def send_transfer(node):
    node.write_message(json.dumps(
        { 'event' : 'transfer' }
    ))

#-------------------------------------------------------------------------------

class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.render('view.html')

#-------------------------------------------------------------------------------

class ViewHandler(tornado.websocket.WebSocketHandler):
    def open(self):
        global view
        if view is None:
            print('view connected')
            view = self
            send_info()

    def on_message(self, message):
        global nodes, nodes_ready, queue

        if isinstance(message, str):

            data = json.loads(message)

            if 'event' in data:

                event = data['event']

                if event == 'start':
                    print('received start')
                    for node in nodes:
                        send_nudge(node)

                if event == 'stop':
                    print('received stop')

                elif event == 'calculate':
                    print('received calculate')
                    node = nodes_ready.pop()
                    queue.append(node)
                    node.write_message(message)

    def on_close(self):
        global view
        if view is self:
            print('view disconnected')
            view = None

#-------------------------------------------------------------------------------

class NodeHandler(tornado.websocket.WebSocketHandler):
    alive = True
    waiting = False

    def open(self):
        global nodes
        print('node connected')
        nodes.add(self)
        send_info()

    def on_message(self, message):
        global nodes_ready, queue

        if isinstance(message, str):

            data = json.loads(message)

            if 'event' in data:

                event = data['event']

                if event == 'ready':
                    print('received ready')
                    self.waiting = False
                    nodes_ready.add(self)
                    send_ready()

                elif event == 'waiting':
                    print('received waiting')
                    self.waiting = True
                    while queue:
                        if queue[0].waiting:
                            node = queue.popleft()
                            if node.alive:
                                send_transfer(node)
                        else: break

        else:

            print('transmitting frame to view')

            view.write_message(message, binary = True)

    def on_close(self):
        global nodes, nodes_ready, queue
        print('node disconnected')
        self.alive = False
        nodes.remove(self)
        if self in nodes_ready:
            nodes_ready.remove(self)
        if self in queue:
            while self in queue:
                queue.remove(self)
        send_info()

#-------------------------------------------------------------------------------

root_path = os.path.dirname(__file__)

static_path = os.path.join(root_path, 'static')

app = tornado.web.Application([
    (r'/', MainHandler),
    (r'/static/(.*)', tornado.web.StaticFileHandler, {'path': static_path}),
    (r'/view', ViewHandler),
    (r'/node', NodeHandler)
])

if __name__ == "__main__":
    app.listen(8080)
    tornado.ioloop.IOLoop.instance().start()

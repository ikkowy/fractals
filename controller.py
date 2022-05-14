#!/usr/bin/env python3

import os

from collections import deque

import tornado.ioloop
import tornado.web
import tornado.websocket

import json

#-------------------------------------------------------------------------------

view = None
nodes_count = 0
nodes_ready = deque()
nodes_working = deque()

#-------------------------------------------------------------------------------

def send_info():
    global view
    if view is not None:
        view.write_message(json.dumps(
            {
                'action' : 'info',
                'nodes_count' : nodes_count
            }
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
        self.write_message(message)

    def on_close(self):
        global view
        if view is self:
            print('view disconnected')
            view = None

#-------------------------------------------------------------------------------

class NodeHandler(tornado.websocket.WebSocketHandler):
    alive = True

    def open(self):
        global nodes_ready, nodes_count
        print('node connected')
        nodes_ready.appendleft(self)
        nodes_count += 1
        send_info()

    def on_message(self, message):
        if view is not None:
            view.write_message(message)

    def on_close(self):
        global nodes_count
        print('node disconnected')
        alive = False
        nodes_count -= 1
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

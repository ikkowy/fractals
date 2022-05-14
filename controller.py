#!/usr/bin/env python3

import os

import tornado.ioloop
import tornado.web
import tornado.websocket

class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.render('view.html')

class ViewHandler(tornado.websocket.WebSocketHandler):
    def open(self):
        print('view connected')

    def on_message(self, message):
        self.write_message(message)

    def on_close(self):
        print('view disconnected')

class NodeHandler(tornado.websocket.WebSocketHandler):
    def open(self):
        print('node connected')

    def on_message(self, message):
        self.write_message(message)

    def on_close(self):
        print('node disconnected')

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

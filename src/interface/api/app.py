# Copyright 2021 Marzuk Rashid

"""Webserver for the FISHBAIT web interface."""

from flask import Flask

app = Flask(__name__)

@app.route('/')
def hello_world():
  return '<h1>Hello, World!</h1>'

if __name__ == '__main__':
  app.run(debug=True)

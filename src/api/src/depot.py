'''A process & thread safe storage service for Fishbait user sessions'''

from depot_types import depot_server

if __name__ == '__main__':
  s = depot_server.get_server()
  s.serve_forever()

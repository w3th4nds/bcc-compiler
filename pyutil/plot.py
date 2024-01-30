#!/usr/bin/env python3

class node(object):
	def __init__(self, value, children = []):
		self.value = value
		self.children = children

	def __str__(self, level=0):
		ret = "\t"*level+repr(self.value)+"\n"
		for child in self.children:
			ret += child.__str__(level+1)
		return ret

	def __repr__(self):
		return '<tree node representation>'

def make_graph():
  with open('AST_BFS.txt', 'r') as f:
    lines = [l.strip().split(',')[:-1] for l in f.readlines()]

  target = dict()
  for l in lines:
    target.update({l[0]: l[1:]})
  print(target)

  graph = Graph(target, directed=True)
  graph.plot(orientation='UD', fill_color='#aec6cf')
  webbrowser.open('graph.png')

    
if __name__ == '__main__':
  make_graph()

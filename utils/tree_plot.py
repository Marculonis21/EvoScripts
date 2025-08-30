#!/usr/bin/env python

import sys 
import plotly.graph_objects as go

assert(len(sys.argv) == 2 and "We expect evodex file as input");

class DexEntry:
    def __init__(self, header):
        # Occurence: 0
        # Handle: 13088 Parent: 13045
        # DNAPre: 13088, 12661, 12586, 11591, 11561, 11232, 11207, 11171, 11137, 11084, 11053, 11021, 10980, 10917, 10877, 10815, 10768, 9622, 9451, 
        # DOB: 0
        # Instructions (lenght: 83)
        print(header)
        self._get_occurence(header[0])
        self._get_handles(header[1])
        self._get_history(header[2])
        self._get_instructions(header[4])

        print(self.ID, self.pID, self.history, self.instr_lenght, self.occurence)

    def _get_handles(self, line):
        parts = line.split(" ")
        self.ID = int(parts[1])
        self.pID = int(parts[3])

    def _get_history(self, line):
        self.history = [int(x) for x in line.removeprefix("DNAPre: ").split(",") if x != ""]
        self.label = self.history[0]
        self.label_parent = self.history[-1]
        if self.label == 0:
            # hardcode it now
            self.label_parent = 0

    def _get_instructions(self, line):
        parts = line.split(" ")
        self.instr_lenght = (parts[2].removesuffix(')'))

    def _get_occurence(self, line):
        parts = line.split(" ")
        self.occurence = (parts[1])

print("Evodex path: ", sys.argv[1])

dex_entries = []
with open(sys.argv[1], "r") as f:
    while True:
        line = f.readline(-1).strip()

        if line == "---- Memory print out ----":
            break

        if line == "ENTRY:":
            header = []
            for _ in range(5):
                header.append(f.readline(-1).strip())

            dex_entries.append(DexEntry(header))

# fig = go.Figure(go.Treemap(
#     labels = [x.label for x in dex_entries],
#     parents = [x.label_parent for x in dex_entries],
#     root_color="lightgrey"
# ))
# fig.update_layout(margin = dict(t=50, l=25, r=25, b=25))
# fig.show()

import igraph as ig
import matplotlib.pyplot as plt

# Create a graph object
g = ig.Graph(directed=True)

# Add vertices (nodes) - 7 nodes for a simple tree
g.add_vertices(len(dex_entries))

labels = [x.label for x in dex_entries]
g.vs["label"] = labels

# Add edges (connections) to make it a tree (no cycles)
edges = [(x.label_parent, x.label) for x in dex_entries]
edges.sort(key=lambda x: (x[0], x[1]))

for i, (parent, target) in enumerate(edges):
    print(f"{i}/{len(edges)}")
    _from = g.vs.find(label=parent)
    _to   = g.vs.find(label=target)
    g.add_edge(_from, _to)


# Layout the tree - hierarchical layout for a tree structure
layout = g.layout("tree")

# Plot the tree using the layout
fig, ax = plt.subplots()
ig.plot(g, layout=layout, target=ax, vertex_size=20, vertex_label=g.vs["label"], vertex_color=["green" if v["label"] == 0 else "red" for v in g.vs])

# Display the plot
plt.tight_layout()
plt.show()

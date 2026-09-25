#!/usr/bin/env python3
import sys
import json
import os
import networkx as nx
import matplotlib.pyplot as plt

def hierarchy_pos(G, root=None, width=1., vert_gap=0.2, vert_loc=0, xcenter=0.5):
    """
    Computes a clean top-to-bottom tree layout for directed graphs.
    """
    def _hierarchy_pos(G, node, left, right, vert_loc, pos, visited):
        visited.add(node)
        pos[node] = ((left + right) / 2, vert_loc)
        neighbors = [n for n in G.successors(node) if n not in visited]
        if neighbors:
            dx = (right - left) / len(neighbors)
            nextx = left
            for neighbor in neighbors:
                _hierarchy_pos(G, neighbor, nextx, nextx + dx, vert_loc - vert_gap, pos, visited)
                nextx += dx

    pos = {}
    visited = set()
    roots = [n for n, d in G.in_degree() if d == 0] if root is None else [root]
    
    if not roots:
        roots = list(G.nodes())[:1]

    dx = width / len(roots)
    nextx = 0
    for r in roots:
        _hierarchy_pos(G, r, nextx, nextx + dx, vert_loc, pos, visited)
        nextx += dx

    return pos

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 utils/tree_plot.py <evodex.json>")
        sys.exit(1)

    filepath = sys.argv[1]
    if not os.path.exists(filepath):
        print(f"Error: File not found: {filepath}")
        sys.exit(1)

    print(f"Loading EvoDex JSON from: {filepath}")
    with open(filepath, "r") as f:
        data = json.load(f)

    entries = data.get("evodex", [])
    if not entries:
        print("No recorded species found in the evodex JSON.")
        return

    print(f"Loaded {len(entries)} recorded species.")

    # Create NetworkX Directed Graph
    G = nx.DiGraph()

    for entry in entries:
        target = entry["handle"]
        dna_pre = entry.get("dna_pre", [])
        parent = dna_pre[-1] if len(dna_pre) > 1 else entry.get("parent", 0)

        G.add_node(target, 
                   length=entry.get("genome_length", "?"), 
                   occurence=entry.get("occurence", 1))

        if parent not in G:
            G.add_node(parent, length="?", occurence=1)

        if parent != target:
            G.add_edge(parent, target)

    if G.number_of_edges() == 0 and G.number_of_nodes() <= 1:
        print("Only ancestor present, no branching links to plot.")
        return

    # Calculate hierarchical tree layout
    try:
        pos = hierarchy_pos(G)
    except Exception:
        pos = nx.spring_layout(G, seed=42)

    # Node styling
    node_colors = ["#4CAF50" if node == 0 else "#2196F3" for node in G.nodes()]
    labels = {node: f"{node}\n(L={G.nodes[node].get('length', '?')})" for node in G.nodes()}

    fig, ax = plt.subplots(figsize=(14, 9))

    nx.draw_networkx_nodes(G, pos, ax=ax, node_color=node_colors, node_size=1500, edgecolors="#333333")
    nx.draw_networkx_labels(G, pos, labels=labels, ax=ax, font_size=8, font_family="sans-serif")
    nx.draw_networkx_edges(
        G, pos, ax=ax, 
        edge_color="#888888", 
        arrows=True, 
        arrowsize=15, 
        arrowstyle="-|>", 
        node_size=1500
    )

    ax.set_title(f"EvoScripts Phylogenetic Tree ({G.number_of_nodes()} species)", fontsize=14, fontweight="bold")
    ax.axis("off")
    plt.tight_layout()
    plt.show()

if __name__ == "__main__":
    main()

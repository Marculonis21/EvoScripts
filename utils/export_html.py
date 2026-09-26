#!/usr/bin/env python3
import sys
import json
import os
import webbrowser
import argparse

HTML_TEMPLATE = r"""<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <title>EvoScripts Genome Explorer</title>
  <script type="text/javascript" src="https://unpkg.com/vis-network/standalone/umd/vis-network.min.js"></script>
  <style>
    :root {
      --bg-color: #0d1117;
      --card-bg: #161b22;
      --border-color: #30363d;
      --text-color: #c9d1d9;
      --text-muted: #8b949e;
      --accent: #58a6ff;
      --accent-green: #238636;
      --diff-add-bg: #1f3d26;
      --diff-add-border: #3fb950;
      --diff-del-bg: #441c24;
      --diff-del-border: #f85149;
      --diff-mod-bg: #3d3118;
      --diff-mod-border: #d29922;
    }
    * { box-sizing: border-box; margin: 0; padding: 0; }
    body {
      font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif;
      background-color: var(--bg-color);
      color: var(--text-color);
      display: flex;
      flex-direction: column;
      height: 100vh;
      overflow: hidden;
    }
    header {
      background: var(--card-bg);
      border-bottom: 1px solid var(--border-color);
      padding: 12px 24px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      flex-shrink: 0;
    }
    h1 { font-size: 1.15rem; font-weight: 600; display: flex; align-items: center; gap: 8px; }
    .badge {
      background: #21262d;
      padding: 2px 8px;
      border-radius: 12px;
      font-size: 0.75rem;
      border: 1px solid var(--border-color);
      color: var(--accent);
    }
    .layout-main {
      display: flex;
      flex: 1;
      min-height: 0;
      position: relative;
    }
    .left-pane {
      width: 48%;
      min-width: 320px;
      display: flex;
      flex-direction: column;
      min-height: 0;
      background: var(--bg-color);
    }
    #network-pane {
      flex: 1;
      min-height: 220px;
      position: relative;
      background: #090d13;
      display: flex;
      flex-direction: column;
    }
    .pane-header {
      padding: 6px 14px;
      background: #161b22;
      border-bottom: 1px solid var(--border-color);
      font-size: 0.8rem;
      font-weight: 600;
      color: var(--text-color);
      display: flex;
      justify-content: space-between;
      align-items: center;
      flex-wrap: wrap;
      gap: 8px;
      flex-shrink: 0;
    }
    .tree-controls {
      display: flex;
      align-items: center;
      gap: 8px;
      font-size: 0.75rem;
    }
    .tree-controls label {
      display: flex;
      align-items: center;
      gap: 4px;
      color: var(--text-muted);
    }
    .tree-controls select {
      padding: 2px 6px;
      font-size: 0.75rem;
      background: #0d1117;
      color: var(--text-color);
      border: 1px solid var(--border-color);
      border-radius: 4px;
      max-width: 125px;
    }
    .btn-icon {
      width: auto;
      margin-top: 0;
      padding: 2px 7px;
      font-size: 0.8rem;
      line-height: 1.2;
    }
    .tree-legend {
      padding: 4px 12px;
      background: #12171f;
      border-top: 1px solid var(--border-color);
      display: flex;
      align-items: center;
      gap: 12px;
      font-size: 0.72rem;
      color: var(--text-muted);
      flex-shrink: 0;
      flex-wrap: wrap;
    }
    .legend-item {
      display: flex;
      align-items: center;
      gap: 5px;
    }
    .legend-dot {
      width: 9px;
      height: 9px;
      border-radius: 50%;
      border: 1px solid transparent;
      display: inline-block;
    }
    #network-container {
      width: 100%;
      flex: 1;
      min-height: 0;
    }
    #sidebar-pane {
      height: 230px;
      background: var(--card-bg);
      border-top: 1px solid var(--border-color);
      display: flex;
      flex-direction: column;
      flex-shrink: 0;
    }
    .sidebar-header {
      padding: 8px 14px;
      border-bottom: 1px solid var(--border-color);
      font-weight: 600;
      font-size: 0.85rem;
      display: flex;
      justify-content: space-between;
      align-items: center;
      flex-shrink: 0;
    }
    .sidebar-content {
      padding: 10px 14px;
      overflow-y: auto;
      flex: 1;
    }
    .meta-table {
      width: 100%;
      font-size: 0.8rem;
      border-collapse: collapse;
      margin-bottom: 6px;
    }
    .meta-table td {
      padding: 3px 0;
      border-bottom: 1px solid #21262d;
    }
    .meta-table td:first-child { color: var(--text-muted); width: 45%; }
    .btn {
      background: #21262d;
      color: var(--text-color);
      border: 1px solid var(--border-color);
      padding: 6px 12px;
      border-radius: 6px;
      cursor: pointer;
      font-size: 0.82rem;
      font-weight: 500;
      transition: all 0.2s;
      width: 100%;
      margin-top: 5px;
    }
    .btn:hover { background: #30363d; border-color: #8b949e; }
    .btn-primary {
      background: var(--accent-green);
      border-color: rgba(240,246,252,0.1);
      color: #fff;
    }
    .btn-primary:hover { background: #2ea043; }

    /* Divider Resizer */
    .resizer-col {
      width: 5px;
      background: var(--border-color);
      cursor: col-resize;
      transition: background 0.15s;
      flex-shrink: 0;
      z-index: 10;
    }
    .resizer-col:hover, .resizer-col.resizing {
      background: var(--accent);
    }

    /* Right Pane: Full Height Vertical Diff View */
    #diff-panel {
      flex: 1;
      min-width: 360px;
      background: var(--card-bg);
      display: flex;
      flex-direction: column;
      min-height: 0;
      height: 100%;
    }
    .diff-header {
      padding: 10px 14px;
      background: #161b22;
      border-bottom: 1px solid var(--border-color);
      display: flex;
      justify-content: space-between;
      align-items: center;
      flex-wrap: wrap;
      gap: 10px;
      flex-shrink: 0;
    }
    .diff-selector-group {
      display: flex;
      align-items: center;
      gap: 8px;
      flex-wrap: wrap;
      font-size: 0.82rem;
    }
    select {
      background: #0d1117;
      color: var(--text-color);
      border: 1px solid var(--border-color);
      padding: 4px 6px;
      border-radius: 4px;
      font-size: 0.82rem;
      max-width: 190px;
    }
    .btn-compare {
      width: auto;
      margin-top: 0;
      padding: 4px 12px;
      background: #21262d;
    }
    .btn-compare:hover {
      background: #30363d;
      border-color: var(--accent);
    }
    .diff-summary-badge {
      font-size: 0.8rem;
      color: var(--text-muted);
      font-family: ui-monospace, SFMono-Regular, "SF Mono", Menlo, Consolas, monospace;
    }
    .diff-columns-wrapper {
      display: flex;
      flex: 1;
      min-height: 0;
      overflow-y: auto;
      overflow-x: hidden;
      font-family: ui-monospace, SFMono-Regular, "SF Mono", Menlo, Consolas, monospace;
      font-size: 0.82rem;
      background: #090d13;
    }
    .diff-col {
      flex: 1;
      min-width: 0;
      border-right: 1px solid var(--border-color);
      display: flex;
      flex-direction: column;
    }
    .diff-col:last-child { border-right: none; }
    .diff-col-header {
      position: sticky;
      top: 0;
      background: #161b22;
      padding: 8px 12px;
      font-weight: 600;
      border-bottom: 1px solid var(--border-color);
      font-size: 0.78rem;
      color: var(--accent);
      z-index: 2;
    }
    .diff-line {
      display: flex;
      padding: 2px 8px;
      white-space: pre;
      line-height: 1.45;
      border-left: 3px solid transparent;
      transition: background 0.08s;
    }
    .diff-line.same { color: var(--text-muted); }
    .diff-line.add {
      background: var(--diff-add-bg);
      border-left-color: var(--diff-add-border);
      color: #7ee787;
    }
    .diff-line.del {
      background: var(--diff-del-bg);
      border-left-color: var(--diff-del-border);
      color: #ff7b72;
    }
    .diff-line.empty {
      background: rgba(0, 0, 0, 0.25);
      color: transparent;
    }
    .diff-line.diff-row-hover {
      background-color: rgba(88, 166, 255, 0.16) !important;
    }
    .line-no {
      width: 36px;
      text-align: right;
      padding-right: 10px;
      color: #484f58;
      user-select: none;
      flex-shrink: 0;
    }
    .line-op {
      width: 44px;
      color: #8b949e;
      flex-shrink: 0;
    }
    .line-name {
      font-weight: 500;
      color: #e6edf3;
      overflow: hidden;
      text-overflow: ellipsis;
      white-space: nowrap;
    }
  </style>
</head>
<body>

  <header>
    <h1>
      <span>🌱 EvoScripts Genome Explorer</span>
      <span class="badge" id="species-count-badge">0 species</span>
    </h1>
    <div style="font-size: 0.8rem; color: var(--text-muted)">
      Click any node to inspect & compare genomes
    </div>
  </header>

  <div class="layout-main">
    <!-- Left Pane: Tree & Inspector -->
    <div class="left-pane" id="left-pane">
      <div id="network-pane">
        <div class="pane-header">
          <div style="display: flex; align-items: center; gap: 8px;">
            <span>Lineage Tree</span>
            <span class="badge" id="visible-count-badge">0 / 0</span>
          </div>
          <div class="tree-controls">
            <label>
              Filter:
              <select id="filter-occ" onchange="rebuildGraph()">
                <option value="0">All</option>
                <option value="2">Viable (&ge;2)</option>
                <option value="5">Established (&ge;5)</option>
                <option value="10">Dominant (&ge;10)</option>
              </select>
            </label>
            <label>
              Layout:
              <select id="layout-select" onchange="changeLayout()">
                <option value="LR" selected>Tree (Left &rarr; Right)</option>
                <option value="UD">Tree (Top &darr; Down)</option>
                <option value="spring">Spring (Radial 360&deg;)</option>
                <option value="forceAtlas2">Organic (ForceAtlas2)</option>
              </select>
            </label>
            <button class="btn btn-icon" onclick="resetZoom()" title="Reset Zoom / Fit View">⟲</button>
          </div>
        </div>
        <div id="network-container"></div>
        <div class="tree-legend">
          <span class="legend-item"><span class="legend-dot" style="background: #238636; border-color: #3fb950;"></span> Founder</span>
          <span class="legend-item"><span class="legend-dot" style="background: #1f6feb; border-color: #58a6ff;"></span> Normal</span>
          <span class="legend-item"><span class="legend-dot" style="background: #8957e5; border-color: #bc8cff;"></span> Parasite (&lt; founder)</span>
          <span class="legend-item"><span class="legend-dot" style="background: #d29922; border-color: #e3b341;"></span> Expanded (&gt; founder)</span>
          <span style="margin-left: auto; color: var(--text-muted);">Size &prop; log(Replicates)</span>
        </div>
      </div>

      <div id="sidebar-pane">
        <div class="sidebar-header">
          <span>Species Inspector</span>
          <span class="badge" id="selected-badge">Select a node</span>
        </div>
        <div class="sidebar-content" id="sidebar-content">
          <p style="color: var(--text-muted); font-size: 0.85rem;">
            Click on any species node in the phylogenetic tree to view its genetic lineage, metadata, and perform code comparisons.
          </p>
        </div>
      </div>
    </div>

    <!-- Draggable resizer -->
    <div class="resizer-col" id="resizer-col"></div>

    <!-- Right Pane: Full-height Code Diff -->
    <div id="diff-panel">
      <div class="diff-header">
        <div class="diff-selector-group">
          <span><strong>Diff:</strong></span>
          <label>A: <select id="diff-select-a"></select></label>
          <span style="color: var(--accent); font-weight: bold;">&rarr;</span>
          <label>B: <select id="diff-select-b"></select></label>
          <button class="btn btn-compare" onclick="renderDiff()">Compare</button>
        </div>
        <div id="diff-summary" class="diff-summary-badge"></div>
      </div>
      <div class="diff-columns-wrapper" id="diff-columns-wrapper">
        <div class="diff-col" id="diff-col-a">
          <div class="diff-col-header" id="diff-header-a">Species A</div>
          <div id="diff-lines-a"></div>
        </div>
        <div class="diff-col" id="diff-col-b">
          <div class="diff-col-header" id="diff-header-b">Species B</div>
          <div id="diff-lines-b"></div>
        </div>
      </div>
    </div>
  </div>

  <script>
    // Embedded Data
    const RAW_DATA = __EMBEDDED_JSON_DATA__;
    const speciesList = RAW_DATA.evodex || [];
    const speciesMap = new Map();
    speciesList.forEach(s => speciesMap.set(s.handle, s));

    document.getElementById('species-count-badge').textContent = `${speciesList.length} species`;

    // Populate dropdowns
    const selectA = document.getElementById('diff-select-a');
    const selectB = document.getElementById('diff-select-b');
    speciesList.forEach(s => {
      const optA = document.createElement('option');
      optA.value = s.handle;
      optA.textContent = `#${s.handle} (L=${s.genome_length}, Occ=${s.occurence})`;
      selectA.appendChild(optA);

      const optB = document.createElement('option');
      optB.value = s.handle;
      optB.textContent = `#${s.handle} (L=${s.genome_length}, Occ=${s.occurence})`;
      selectB.appendChild(optB);
    });

    // Graph Data Builder & Filter
    let currentGraphData = null;

    function buildGraphData(minOcc = 1, isPhysics = false) {
      const filteredSpecies = speciesList.filter(s => s.occurence >= minOcc || s.handle === 0);
      const filteredMap = new Map();
      filteredSpecies.forEach(s => filteredMap.set(s.handle, s));

      const rootSp = speciesMap.get(0);
      const rootLen = rootSp ? rootSp.genome_length : 80;
      const maxOcc = Math.max(...speciesList.map(s => s.occurence), 1);

      const nodes = [];
      const edges = [];

      filteredSpecies.forEach(s => {
        const target = s.handle;

        // Dot Sizing: Logarithmic scale 10px to 32px based on replicates
        const norm = Math.log2(s.occurence + 1) / Math.max(1, Math.log2(maxOcc + 1));
        const size = target === 0 ? 22 : Math.round(9 + norm * 23);

        // Color coding by genome length relative to root
        let colorBg = '#1f6feb', colorBorder = '#58a6ff';
        if (target === 0) {
          colorBg = '#238636'; colorBorder = '#3fb950'; // Founder
        } else if (s.genome_length < rootLen) {
          colorBg = '#8957e5'; colorBorder = '#bc8cff'; // Parasite (shorter)
        } else if (s.genome_length > rootLen) {
          colorBg = '#d29922'; colorBorder = '#e3b341'; // Expanded (longer)
        }

        nodes.push({
          id: target,
          label: `#${target}`,
          title: `Species #${target}\nLength: ${s.genome_length} opcodes\nOccurrences: ${s.occurence}\nDOB: Epoch ${s.dob}`,
          size: size,
          shape: 'dot',
          color: { background: colorBg, border: colorBorder, highlight: { background: '#ffffff', border: '#58a6ff' } },
          font: { color: '#c9d1d9', size: Math.max(10, Math.min(13, Math.round(size * 0.65))), face: 'monospace', vadjust: 2 },
          borderWidth: target === 0 ? 3 : 2
        });

        // Find nearest surviving ancestor in filtered set
        if (target !== 0) {
          let p = 0;
          let intermediateCount = 0;

          // 1. Inspect the recorded phylogenetic lineage chain in dna_pre
          if (s.dna_pre && s.dna_pre.length > 1) {
            for (let i = 1; i < s.dna_pre.length; ++i) {
              const anc = s.dna_pre[i];
              if (filteredMap.has(anc)) {
                p = anc;
                intermediateCount = i - 1;
                break;
              }
            }
          }

          // 2. Fallback to parent pointer chain if not resolved via dna_pre
          if (p === 0 && s.parent !== 0) {
            let curr = s.parent;
            let count = 0;
            while (curr !== 0 && !filteredMap.has(curr)) {
              count++;
              const pObj = speciesMap.get(curr);
              if (!pObj || pObj.parent === curr) { curr = 0; break; }
              curr = pObj.parent;
            }
            if (filteredMap.has(curr)) {
              p = curr;
              intermediateCount = count;
            }
          }

          if (filteredMap.has(p) && p !== target) {
            const edgeObj = {
              from: p,
              to: target,
              arrows: 'to',
              label: intermediateCount > 0 ? `+${intermediateCount}` : '',
              font: { color: '#8b949e', size: 9, align: 'middle' },
              color: { color: '#30363d', highlight: '#58a6ff', hover: '#58a6ff' },
              width: Math.min(3.5, 1.2 + Math.log2(s.occurence + 1) * 0.35),
              selectionWidth: 3
            };

            // In physics mode (spring / forceAtlas2), parameterize springs with genetic divergence.
            // In hierarchical mode, omit length so tree levels stay strictly aligned!
            if (isPhysics) {
              const driftHops = intermediateCount > 0 ? intermediateCount + 1 : 1;
              const springLen = Math.min(180, 50 + driftHops * 15);
              edgeObj.length = springLen;
              edgeObj.springLength = springLen;
              edgeObj.springConstant = Math.min(0.12, 0.04 + Math.log2(s.occurence + 1) * 0.015);
            }

            edges.push(edgeObj);
          }
        }
      });

      return {
        nodes: new vis.DataSet(nodes),
        edges: new vis.DataSet(edges),
        total: filteredSpecies.length
      };
    }

    function getNetworkOptions(layoutMode) {
      if (layoutMode === 'spring') {
        return {
          layout: {
            hierarchical: {
              enabled: false
            }
          },
          physics: {
            enabled: true,
            solver: 'repulsion',
            repulsion: {
              nodeDistance: 85,
              centralGravity: 0.08,
              springLength: 70,
              springConstant: 0.08,
              damping: 0.35
            },
            maxVelocity: 50,
            minVelocity: 0.75,
            timestep: 0.35,
            stabilization: {
              enabled: true,
              iterations: 200,
              updateInterval: 40,
              fit: true
            }
          },
          interaction: { hover: true, selectConnectedEdges: false, zoomView: true, dragView: true }
        };
      }

      if (layoutMode === 'forceAtlas2') {
        return {
          layout: {
            hierarchical: {
              enabled: false
            }
          },
          physics: {
            enabled: true,
            solver: 'forceAtlas2Based',
            forceAtlas2Based: {
              theta: 0.5,
              gravitationalConstant: -60,
              centralGravity: 0.015,
              springLength: 70,
              springConstant: 0.08,
              damping: 0.45,
              avoidOverlap: 0.6
            },
            maxVelocity: 75,
            minVelocity: 1.0,
            timestep: 0.35,
            stabilization: {
              enabled: true,
              iterations: 200,
              updateInterval: 50,
              fit: true
            }
          },
          interaction: { hover: true, selectConnectedEdges: false, zoomView: true, dragView: true }
        };
      }

      const isLR = layoutMode === 'LR';
      return {
        layout: {
          hierarchical: {
            enabled: true,
            direction: isLR ? 'LR' : 'UD',
            sortMethod: 'directed',
            levelSeparation: isLR ? 95 : 75,
            nodeSpacing: isLR ? 30 : 40,
            treeSpacing: 45,
            parentCentralization: true,
            blockShifting: true,
            edgeMinimization: true
          }
        },
        physics: {
          enabled: false
        },
        interaction: { hover: true, selectConnectedEdges: false, zoomView: true, dragView: true }
      };
    }

    // Set default filter based on dataset size
    const defaultMinOcc = speciesList.length > 40 ? 2 : 0;
    const filterSelect = document.getElementById('filter-occ');
    if (filterSelect) filterSelect.value = String(defaultMinOcc);

    currentGraphData = buildGraphData(defaultMinOcc, false);
    document.getElementById('visible-count-badge').textContent = `${currentGraphData.total} / ${speciesList.length}`;

    const container = document.getElementById('network-container');
    const network = new vis.Network(container, { nodes: currentGraphData.nodes, edges: currentGraphData.edges }, getNetworkOptions('LR'));
    network.once('afterDrawing', () => { network.fit(); });
    window.addEventListener('resize', () => { network.fit(); });

    // Auto-freeze organic physics once settled to eliminate endless jitter and save CPU
    network.on('stabilized', function () {
      const mode = document.getElementById('layout-select').value;
      if (mode === 'spring' || mode === 'forceAtlas2') {
        network.setOptions({ physics: { enabled: false } });
      }
    });

    function rebuildGraph() {
      const mode = document.getElementById('layout-select').value;
      const isPhysics = (mode === 'spring' || mode === 'forceAtlas2');
      const minOcc = parseInt(document.getElementById('filter-occ').value, 10);

      currentGraphData = buildGraphData(minOcc, isPhysics);
      const rawNodes = currentGraphData.nodes.get();
      const cleanNodes = isPhysics ? rawNodes.map(n => {
        const copy = Object.assign({}, n);
        delete copy.x;
        delete copy.y;
        return copy;
      }) : rawNodes;

      network.setData({ nodes: new vis.DataSet(cleanNodes), edges: currentGraphData.edges });
      document.getElementById('visible-count-badge').textContent = `${currentGraphData.total} / ${speciesList.length}`;

      if (isPhysics) {
        network.setOptions(getNetworkOptions(mode));
        network.stabilize(200);
      } else {
        setTimeout(() => { network.fit(); }, 120);
      }
    }

    function changeLayout() {
      const mode = document.getElementById('layout-select').value;
      const isPhysics = (mode === 'spring' || mode === 'forceAtlas2');
      const minOcc = parseInt(document.getElementById('filter-occ').value, 10);

      network.setOptions(getNetworkOptions(mode));
      currentGraphData = buildGraphData(minOcc, isPhysics);

      // Strip prior coordinates when switching into physics mode so nodes disperse freely in 360 degrees
      const rawNodes = currentGraphData.nodes.get();
      const cleanNodes = isPhysics ? rawNodes.map(n => {
        const copy = Object.assign({}, n);
        delete copy.x;
        delete copy.y;
        return copy;
      }) : rawNodes;

      network.setData({ nodes: new vis.DataSet(cleanNodes), edges: currentGraphData.edges });

      if (isPhysics) {
        network.stabilize(200);
      }
      setTimeout(() => { network.fit(); }, 200);
    }

    function resetZoom() {
      if (network) network.fit({ animation: { duration: 350, easingFunction: 'easeInOutQuad' } });
    }

    function focusSpecies(handleId) {
      if (network && currentGraphData && currentGraphData.nodes.get(handleId)) {
        network.selectNodes([handleId]);
        network.focus(handleId, {
          scale: 1.15,
          animation: { duration: 350, easingFunction: 'easeInOutQuad' }
        });
      }
    }

    // Dropdown change listeners to sync tree camera
    selectA.addEventListener('change', () => {
      focusSpecies(parseInt(selectA.value, 10));
      renderDiff();
    });
    selectB.addEventListener('change', () => {
      focusSpecies(parseInt(selectB.value, 10));
      renderDiff();
    });

    // Node & Edge Selection
    network.on('click', function(params) {
      if (params.nodes.length > 0) {
        const nodeId = params.nodes[0];
        inspectSpecies(nodeId);
      } else if (params.edges.length > 0) {
        const edgeId = params.edges[0];
        const edge = currentGraphData.edges.get(edgeId);
        if (edge) {
          inspectBranch(edge.from, edge.to, edge.label);
        }
      }
    });

    function inspectBranch(fromId, toId, label) {
      const parent = speciesMap.get(fromId);
      const child = speciesMap.get(toId);
      if (!parent || !child) return;

      document.getElementById('selected-badge').textContent = `#${fromId} → #${toId}`;
      const diffLen = child.genome_length - parent.genome_length;
      const lenSign = diffLen > 0 ? `+${diffLen}` : `${diffLen}`;

      let html = `
        <div style="font-size: 0.8rem; color: var(--accent); margin-bottom: 6px; font-weight: 600;">
          Branch: #${fromId} &rarr; #${toId} (${label || '1 direct step'})
        </div>
        <table class="meta-table">
          <tr><td>Parent Species</td><td><strong>#${fromId}</strong> (${parent.genome_length} opcodes)</td></tr>
          <tr><td>Offspring Species</td><td><strong>#${toId}</strong> (${child.genome_length} opcodes)</td></tr>
          <tr><td>Genome Change</td><td><strong style="color: ${diffLen === 0 ? 'var(--text-color)' : (diffLen < 0 ? '#ff7b72' : '#7ee787')}">${lenSign} opcodes</strong></td></tr>
          <tr><td>Child Birth Epoch</td><td>Epoch ${child.dob}</td></tr>
        </table>
        <div style="display: flex; gap: 8px; margin-top: 8px;">
          <button class="btn" style="margin-top: 0; padding: 4px 8px; font-size: 0.78rem;" onclick="inspectSpecies(${fromId}, false)">Inspect #${fromId}</button>
          <button class="btn" style="margin-top: 0; padding: 4px 8px; font-size: 0.78rem;" onclick="inspectSpecies(${toId}, false)">Inspect #${toId}</button>
        </div>
      `;

      document.getElementById('sidebar-content').innerHTML = html;
      setDiffPair(fromId, toId);
    }

    function inspectSpecies(handleId, autoDiff = true) {
      const s = speciesMap.get(handleId);
      if (!s) return;

      document.getElementById('selected-badge').textContent = `#${handleId}`;
      const dnaPre = s.dna_pre || [];
      const parent = dnaPre.length > 1 ? dnaPre[dnaPre.length - 1] : s.parent;
      const driftHops = dnaPre.length > 1 ? dnaPre.length - 1 : 1;

      let html = `
        <table class="meta-table">
          <tr><td>Species Handle</td><td><strong>#${s.handle}</strong></td></tr>
          <tr><td>Parent Species</td><td><strong>#${parent}</strong></td></tr>
          <tr><td>Genome Length</td><td>${s.genome_length} opcodes</td></tr>
          <tr><td>Occurrences</td><td>${s.occurence} replicates</td></tr>
          <tr><td>Date of Birth</td><td>Epoch ${s.dob}</td></tr>
          <tr><td>Mutational Drift</td><td>${driftHops} intermediate step(s)</td></tr>
        </table>
        <div style="display: flex; gap: 8px; margin-top: 8px;">
          <button class="btn" style="margin-top: 0; padding: 4px 8px; font-size: 0.78rem;" onclick="setAsA(${s.handle})">Set as Diff A</button>
          <button class="btn" style="margin-top: 0; padding: 4px 8px; font-size: 0.78rem;" onclick="setAsB(${s.handle})">Set as Diff B</button>
        </div>
      `;

      document.getElementById('sidebar-content').innerHTML = html;
      if (autoDiff && parent !== undefined && parent !== s.handle) {
        setDiffPair(parent, s.handle);
      }
      focusSpecies(handleId);
    }

    function setAsA(id) { selectA.value = id; focusSpecies(id); renderDiff(); }
    function setAsB(id) { selectB.value = id; focusSpecies(id); renderDiff(); }
    function setDiffPair(idA, idB) {
      selectA.value = idA;
      selectB.value = idB;
      renderDiff();
    }

    // Longest Common Subsequence Diff Algorithm
    function computeLCSDiff(seqA, seqB) {
      const m = seqA.length;
      const n = seqB.length;
      const dp = Array.from({ length: m + 1 }, () => new Int32Array(n + 1));

      for (let i = 0; i < m; ++i) {
        for (let j = 0; j < n; ++j) {
          if (seqA[i].opcode === seqB[j].opcode) {
            dp[i + 1][j + 1] = dp[i][j] + 1;
          } else {
            dp[i + 1][j + 1] = Math.max(dp[i + 1][j], dp[i][j + 1]);
          }
        }
      }

      let i = m, j = n;
      const resultA = [];
      const resultB = [];

      while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && seqA[i - 1].opcode === seqB[j - 1].opcode) {
          resultA.unshift({ type: 'same', op: seqA[i - 1], line: i - 1 });
          resultB.unshift({ type: 'same', op: seqB[j - 1], line: j - 1 });
          i--; j--;
        } else if (j > 0 && (i === 0 || dp[i][j - 1] >= dp[i - 1][j])) {
          resultA.unshift({ type: 'empty' });
          resultB.unshift({ type: 'add', op: seqB[j - 1], line: j - 1 });
          j--;
        } else if (i > 0 && (j === 0 || dp[i][j - 1] < dp[i - 1][j])) {
          resultA.unshift({ type: 'del', op: seqA[i - 1], line: i - 1 });
          resultB.unshift({ type: 'empty' });
          i--;
        }
      }

      return { linesA: resultA, linesB: resultB };
    }

    function renderDiff() {
      const idA = parseInt(selectA.value, 10);
      const idB = parseInt(selectB.value, 10);

      const spA = speciesMap.get(idA);
      const spB = speciesMap.get(idB);

      if (!spA || !spB) return;

      document.getElementById('diff-header-a').textContent = `Species #${idA} (Length: ${spA.genome_length})`;
      document.getElementById('diff-header-b').textContent = `Species #${idB} (Length: ${spB.genome_length})`;

      const diff = computeLCSDiff(spA.instructions || [], spB.instructions || []);

      let htmlA = '';
      let htmlB = '';
      let adds = 0, dels = 0;

      for (let k = 0; k < diff.linesA.length; ++k) {
        const itemA = diff.linesA[k];
        const itemB = diff.linesB[k];

        if (itemA.type === 'empty') {
          htmlA += `<div class="diff-line empty" data-row="${k}"><span class="line-no"> </span></div>`;
        } else {
          const hex = '0x' + itemA.op.opcode.toString(16).padStart(2, '0');
          htmlA += `<div class="diff-line ${itemA.type}" data-row="${k}"><span class="line-no">${itemA.line}</span><span class="line-op">${hex}</span><span class="line-name">${itemA.op.name}</span></div>`;
          if (itemA.type === 'del') dels++;
        }

        if (itemB.type === 'empty') {
          htmlB += `<div class="diff-line empty" data-row="${k}"><span class="line-no"> </span></div>`;
        } else {
          const hex = '0x' + itemB.op.opcode.toString(16).padStart(2, '0');
          htmlB += `<div class="diff-line ${itemB.type}" data-row="${k}"><span class="line-no">${itemB.line}</span><span class="line-op">${hex}</span><span class="line-name">${itemB.op.name}</span></div>`;
          if (itemB.type === 'add') adds++;
        }
      }

      document.getElementById('diff-lines-a').innerHTML = htmlA;
      document.getElementById('diff-lines-b').innerHTML = htmlB;
      document.getElementById('diff-summary').textContent = `Diff: +${adds} added, -${dels} removed`;

      // Reset scroll position on diff comparison
      const wrapper = document.getElementById('diff-columns-wrapper');
      if (wrapper) wrapper.scrollTop = 0;
    }

    // Coupled Line Hover Highlight
    const diffWrapper = document.getElementById('diff-columns-wrapper');
    diffWrapper.addEventListener('mouseover', (e) => {
      const line = e.target.closest('.diff-line');
      if (!line || line.dataset.row === undefined) return;
      const row = line.dataset.row;
      diffWrapper.querySelectorAll(`.diff-line[data-row="${row}"]`).forEach(el => el.classList.add('diff-row-hover'));
    });
    diffWrapper.addEventListener('mouseout', (e) => {
      const line = e.target.closest('.diff-line');
      if (!line || line.dataset.row === undefined) return;
      const row = line.dataset.row;
      diffWrapper.querySelectorAll(`.diff-line[data-row="${row}"]`).forEach(el => el.classList.remove('diff-row-hover'));
    });

    // Horizontal Resizer Dragging
    const resizer = document.getElementById('resizer-col');
    const leftPane = document.getElementById('left-pane');
    let isResizing = false;

    resizer.addEventListener('mousedown', (e) => {
      isResizing = true;
      resizer.classList.add('resizing');
      document.body.style.userSelect = 'none';
      document.body.style.cursor = 'col-resize';
    });

    window.addEventListener('mousemove', (e) => {
      if (!isResizing) return;
      const containerRect = document.querySelector('.layout-main').getBoundingClientRect();
      const newWidth = e.clientX - containerRect.left;
      if (newWidth > 260 && newWidth < containerRect.width - 320) {
        leftPane.style.width = `${newWidth}px`;
        leftPane.style.flex = 'none';
      }
    });

    window.addEventListener('mouseup', () => {
      if (isResizing) {
        isResizing = false;
        resizer.classList.remove('resizing');
        document.body.style.userSelect = '';
        document.body.style.cursor = '';
        if (network) network.fit();
      }
    });

    // Default select first two species
    if (speciesList.length >= 2) {
      selectA.value = speciesList[speciesList.length - 1].handle;
      selectB.value = speciesList[0].handle;
      inspectSpecies(speciesList[0].handle, false);
      renderDiff();
    } else if (speciesList.length === 1) {
      inspectSpecies(speciesList[0].handle, false);
    }
  </script>
</body>
</html>
"""

def main():
    parser = argparse.ArgumentParser(description="Export EvoDex JSON into an interactive Standalone HTML Genome Explorer.")
    parser.add_argument("json_file", help="Path to evodex.json")
    parser.add_argument("-o", "--out", default="evodex_explorer.html", help="Output HTML file path (default: evodex_explorer.html)")
    parser.add_argument("--no-browser", action="store_true", help="Do not automatically open the browser")

    args = parser.parse_args()

    if not os.path.exists(args.json_file):
        print(f"Error: JSON file not found: {args.json_file}")
        sys.exit(1)

    with open(args.json_file, "r") as f:
        data = json.load(f)

    json_str = json.dumps(data)
    html_content = HTML_TEMPLATE.replace("__EMBEDDED_JSON_DATA__", json_str)

    with open(args.out, "w", encoding="utf-8") as f:
        f.write(html_content)

    print(f"Successfully generated interactive Genome Explorer at: {args.out}")
    if not args.no_browser:
        webbrowser.open("file://" + os.path.abspath(args.out))

if __name__ == "__main__":
    main()

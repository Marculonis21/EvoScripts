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
      padding: 10px 20px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      flex-shrink: 0;
      gap: 16px;
    }
    .header-left {
      display: flex;
      align-items: center;
      gap: 12px;
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
    .badge-green { color: #3fb950; border-color: rgba(63, 185, 80, 0.4); }
    .tab-bar {
      display: flex;
      background: #090d13;
      padding: 3px;
      border-radius: 8px;
      border: 1px solid var(--border-color);
      gap: 4px;
    }
    .tab-btn {
      background: transparent;
      border: none;
      color: var(--text-muted);
      padding: 5px 12px;
      border-radius: 6px;
      font-size: 0.78rem;
      font-weight: 600;
      cursor: pointer;
      display: flex;
      align-items: center;
      gap: 6px;
      transition: all 0.15s ease;
    }
    .tab-btn:hover { color: var(--text-color); background: rgba(255, 255, 255, 0.05); }
    .tab-btn.active {
      color: #ffffff;
      background: #21262d;
      box-shadow: 0 1px 3px rgba(0, 0, 0, 0.3);
    }
    .layout-main {
      display: flex;
      flex: 1;
      min-height: 0;
      position: relative;
    }
    .view-container {
      display: none;
      flex: 1;
      width: 100%;
      height: 100%;
      min-height: 0;
    }
    .view-container.active { display: flex; }

    /* Left Pane: Tree & Inspector */
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
      flex-wrap: wrap;
    }
    .tree-controls label {
      display: flex;
      align-items: center;
      gap: 4px;
      color: var(--text-muted);
    }
    select, input[type="text"], input[type="number"] {
      background: #21262d;
      color: var(--text-color);
      border: 1px solid var(--border-color);
      border-radius: 6px;
      padding: 3px 8px;
      font-size: 0.75rem;
      outline: none;
    }
    select:focus, input:focus { border-color: var(--accent); }
    .btn {
      background: #21262d;
      color: var(--text-color);
      border: 1px solid var(--border-color);
      border-radius: 6px;
      padding: 4px 10px;
      font-size: 0.75rem;
      cursor: pointer;
      display: inline-flex;
      align-items: center;
      justify-content: center;
      transition: background 0.15s, border-color 0.15s;
    }
    .btn:hover { background: #30363d; border-color: #8b949e; }
    .btn-icon { padding: 4px 8px; font-weight: bold; }
    .btn-primary { background: #238636; border-color: #3fb950; color: #ffffff; }
    .btn-primary:hover { background: #2ea043; }

    #network-container {
      flex: 1;
      width: 100%;
      height: 100%;
      min-height: 0;
    }
    .tree-legend {
      padding: 6px 14px;
      background: #161b22;
      border-top: 1px solid var(--border-color);
      font-size: 0.72rem;
      display: flex;
      align-items: center;
      gap: 12px;
      flex-wrap: wrap;
      flex-shrink: 0;
    }
    .legend-item { display: flex; align-items: center; gap: 5px; }
    .legend-dot { width: 10px; height: 10px; border-radius: 50%; border: 1.5px solid transparent; }

    #sidebar-pane {
      height: 230px;
      min-height: 150px;
      max-height: 380px;
      background: var(--card-bg);
      border-top: 1px solid var(--border-color);
      display: flex;
      flex-direction: column;
      flex-shrink: 0;
    }
    .sidebar-header {
      padding: 8px 14px;
      background: #1c2128;
      border-bottom: 1px solid var(--border-color);
      font-size: 0.8rem;
      font-weight: 600;
      color: var(--text-color);
      display: flex;
      justify-content: space-between;
      align-items: center;
    }
    .sidebar-content {
      padding: 12px 16px;
      overflow-y: auto;
      flex: 1;
      font-size: 0.82rem;
    }
    .meta-table { width: 100%; border-collapse: collapse; margin-bottom: 8px; }
    .meta-table td { padding: 3px 6px; border-bottom: 1px solid rgba(48, 54, 61, 0.4); }
    .meta-table td:first-child { color: var(--text-muted); width: 130px; }

    /* Resizer */
    .resizer-col {
      width: 6px;
      background: var(--border-color);
      cursor: col-resize;
      transition: background 0.15s;
      flex-shrink: 0;
    }
    .resizer-col:hover, .resizer-col.resizing { background: var(--accent); }

    /* Right Pane: Code Diff */
    #diff-panel {
      flex: 1;
      min-width: 320px;
      background: var(--bg-color);
      display: flex;
      flex-direction: column;
      min-height: 0;
    }
    .diff-header {
      padding: 8px 16px;
      background: var(--card-bg);
      border-bottom: 1px solid var(--border-color);
      display: flex;
      justify-content: space-between;
      align-items: center;
      gap: 12px;
      flex-shrink: 0;
    }
    .diff-selector-group {
      display: flex;
      align-items: center;
      gap: 8px;
      font-size: 0.8rem;
    }
    .diff-summary-badge {
      font-size: 0.75rem;
      color: var(--text-muted);
      background: #21262d;
      padding: 2px 8px;
      border-radius: 10px;
      border: 1px solid var(--border-color);
    }
    .diff-columns-wrapper {
      display: flex;
      flex: 1;
      min-height: 0;
      overflow-y: auto;
      background: #090d13;
    }
    .diff-col {
      flex: 1;
      min-width: 0;
      border-right: 1px solid var(--border-color);
      font-family: ui-monospace, SFMono-Regular, "SF Mono", Menlo, Consolas, monospace;
      font-size: 0.78rem;
    }
    .diff-col:last-child { border-right: none; }
    .diff-col-header {
      padding: 6px 12px;
      background: #161b22;
      border-bottom: 1px solid var(--border-color);
      font-weight: 600;
      color: var(--text-muted);
      position: sticky;
      top: 0;
      z-index: 10;
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
    .diff-line.add { background: var(--diff-add-bg); border-left-color: var(--diff-add-border); color: #7ee787; }
    .diff-line.del { background: var(--diff-del-bg); border-left-color: var(--diff-del-border); color: #ff7b72; }
    .diff-line.empty { background: rgba(0, 0, 0, 0.25); color: transparent; }
    .diff-line.diff-row-hover { background-color: rgba(88, 166, 255, 0.16) !important; }
    .line-no { width: 36px; text-align: right; padding-right: 10px; color: #484f58; user-select: none; flex-shrink: 0; }
    .line-op { width: 44px; color: #8b949e; flex-shrink: 0; }
    .line-name { font-weight: 500; color: #e6edf3; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; }

    /* Timeline & Scatter View */
    #timeline-view {
      flex-direction: column;
      padding: 16px;
      gap: 12px;
      background: var(--bg-color);
      overflow-y: auto;
    }
    .timeline-card {
      background: var(--card-bg);
      border: 1px solid var(--border-color);
      border-radius: 8px;
      padding: 16px;
      display: flex;
      flex-direction: column;
      gap: 10px;
    }
    .timeline-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
    }
    .timeline-canvas-wrapper {
      position: relative;
      width: 100%;
      height: 480px;
      background: #090d13;
      border-radius: 6px;
      border: 1px solid var(--border-color);
      overflow: hidden;
    }
    #timeline-canvas { width: 100%; height: 100%; display: block; cursor: crosshair; }
    .scatter-tooltip {
      position: absolute;
      display: none;
      background: #1c2128;
      border: 1px solid var(--accent);
      border-radius: 6px;
      padding: 8px 12px;
      font-size: 0.78rem;
      pointer-events: none;
      z-index: 100;
      box-shadow: 0 4px 12px rgba(0,0,0,0.5);
    }

    /* Leaderboard View */
    #leaderboard-view {
      flex-direction: column;
      padding: 16px 24px;
      gap: 16px;
      background: var(--bg-color);
      overflow-y: auto;
    }
    .table-card {
      background: var(--card-bg);
      border: 1px solid var(--border-color);
      border-radius: 8px;
      overflow: hidden;
    }
    .data-table {
      width: 100%;
      border-collapse: collapse;
      font-size: 0.82rem;
      text-align: left;
    }
    .data-table th {
      background: #1c2128;
      padding: 10px 14px;
      color: var(--text-muted);
      border-bottom: 1px solid var(--border-color);
      font-weight: 600;
    }
    .data-table td {
      padding: 8px 14px;
      border-bottom: 1px solid rgba(48, 54, 61, 0.4);
    }
    .data-table tr:hover { background: rgba(88, 166, 255, 0.05); }
    .bar-cell {
      display: flex;
      align-items: center;
      gap: 8px;
    }
    .bar-fill {
      height: 6px;
      background: var(--accent);
      border-radius: 3px;
      min-width: 4px;
    }
  </style>
</head>
<body>

  <header>
    <div class="header-left">
      <h1>
        <span>🌱 EvoScripts Genome Explorer</span>
        <span class="badge" id="species-count-badge">0 species</span>
        <span class="badge badge-green" id="prune-badge" style="display:none;">Pruned Lineage</span>
      </h1>
    </div>

    <!-- View Switcher Tabs -->
    <div class="tab-bar">
      <button class="tab-btn active" onclick="switchView('tree')" id="tab-btn-tree">🌳 Lineage Tree & Diff</button>
      <button class="tab-btn" onclick="switchView('timeline')" id="tab-btn-timeline">📈 Genome Evolution Timeline</button>
      <button class="tab-btn" onclick="switchView('leaderboard')" id="tab-btn-leaderboard">🏆 Species Leaderboard</button>
    </div>

    <div style="font-size: 0.8rem; color: var(--text-muted);">
      Interactive Clade Analysis
    </div>
  </header>

  <!-- VIEW 1: Lineage Tree & Diff -->
  <div class="view-container active" id="view-tree">
    <div class="layout-main" style="width: 100%;">
      <!-- Left Pane: Tree & Inspector -->
      <div class="left-pane" id="left-pane">
        <div id="network-pane">
          <div class="pane-header">
            <div style="display: flex; align-items: center; gap: 8px;">
              <span>Phylogenetic Tree</span>
              <span class="badge" id="visible-count-badge">0 / 0</span>
            </div>
            <div class="tree-controls">
              <label title="Filter by occurrence or clade importance">
                Show:
                <select id="filter-occ" onchange="rebuildGraph()">
                  <option value="top25">Top 25 Clades</option>
                  <option value="top50">Top 50 Clades</option>
                  <option value="top100" selected>Top 100 Clades</option>
                  <option value="top250">Top 250 Clades</option>
                  <option value="1000">Apex (&ge;1,000)</option>
                  <option value="100">Dominant (&ge;100)</option>
                  <option value="20">Established (&ge;20)</option>
                  <option value="5">Viable (&ge;5)</option>
                  <option value="0">All Loaded</option>
                </select>
              </label>

              <label title="Preserve ancestral paths so the tree stays fully connected back to root">
                <input type="checkbox" id="check-keep-trunk" checked onchange="rebuildGraph()">
                Trunk
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

              <div style="display: flex; align-items: center; gap: 3px;">
                <input type="number" id="search-id-input" placeholder="ID..." style="width: 65px;" onkeydown="if(event.key==='Enter') searchSpecies()">
                <button class="btn btn-icon" onclick="searchSpecies()" title="Find & Focus Species">🔍</button>
              </div>

              <button class="btn btn-icon" onclick="resetZoom()" title="Reset Zoom / Fit View">⟲</button>
            </div>
          </div>
          <div id="network-container"></div>
          <div class="tree-legend">
            <span class="legend-item"><span class="legend-dot" style="background: #238636; border-color: #3fb950;"></span> Founder</span>
            <span class="legend-item"><span class="legend-dot" style="background: #1f6feb; border-color: #58a6ff;"></span> Normal</span>
            <span class="legend-item"><span class="legend-dot" style="background: #8957e5; border-color: #bc8cff;"></span> Compact / Parasite (&lt; founder)</span>
            <span class="legend-item"><span class="legend-dot" style="background: #d29922; border-color: #e3b341;"></span> Expanded (&gt; founder)</span>
            <span style="margin-left: auto; color: var(--text-muted);">Dbl-click node to collapse/expand branch</span>
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
  </div>

  <!-- VIEW 2: Genome Evolution Timeline (Scatter Plot) -->
  <div class="view-container" id="view-timeline">
    <div id="timeline-view">
      <div class="timeline-card">
        <div class="timeline-header">
          <div>
            <h2 style="font-size: 1rem; color: #fff;">Genome Length vs. Date of Birth (Epoch)</h2>
            <div style="font-size: 0.75rem; color: var(--text-muted);">
              Visualizing the emergence of parasites, code compaction, and genome expansions over evolutionary time.
            </div>
          </div>
          <div style="display: flex; gap: 8px; font-size: 0.75rem;">
            <span class="legend-item"><span class="legend-dot" style="background: #238636;"></span> Founder</span>
            <span class="legend-item"><span class="legend-dot" style="background: #8957e5;"></span> Parasite (&lt; founder)</span>
            <span class="legend-item"><span class="legend-dot" style="background: #1f6feb;"></span> Normal</span>
            <span class="legend-item"><span class="legend-dot" style="background: #d29922;"></span> Expanded</span>
          </div>
        </div>
        <div class="timeline-canvas-wrapper" id="canvas-wrapper">
          <canvas id="timeline-canvas"></canvas>
          <div class="scatter-tooltip" id="scatter-tooltip"></div>
        </div>
        <div style="font-size: 0.75rem; color: var(--text-muted); display: flex; justify-content: space-between;">
          <span>Bubble size &prop; log(replications). Click any bubble to inspect and load its genome diff.</span>
          <span id="scatter-status">Hover over a point for details</span>
        </div>
      </div>
    </div>
  </div>

  <!-- VIEW 3: Species Leaderboard -->
  <div class="view-container" id="view-leaderboard">
    <div id="leaderboard-view">
      <div class="table-card">
        <div class="pane-header" style="padding: 10px 16px;">
          <span>Dominant Species Leaderboard (Top Replicators)</span>
          <span class="badge" id="leaderboard-count-badge">0 entries</span>
        </div>
        <table class="data-table" id="leaderboard-table">
          <thead>
            <tr>
              <th style="width: 50px;">Rank</th>
              <th style="width: 90px;">Handle</th>
              <th style="width: 140px;">Genome Length</th>
              <th>Replications & Population Share</th>
              <th style="width: 110px;">Birth Epoch</th>
              <th style="width: 90px;">Parent</th>
              <th style="width: 120px;">Action</th>
            </tr>
          </thead>
          <tbody id="leaderboard-body"></tbody>
        </table>
      </div>
    </div>
  </div>

  <script>
    // Embedded Data
    const RAW_DATA = __EMBEDDED_JSON_DATA__;
    const speciesList = RAW_DATA.evodex || [];
    const speciesMap = new Map();
    speciesList.forEach(s => speciesMap.set(s.handle, s));

    const totalRawCount = RAW_DATA.total_raw_count || speciesList.length;
    document.getElementById('species-count-badge').textContent = `${speciesList.length} species`;
    if (totalRawCount > speciesList.length) {
      const pBadge = document.getElementById('prune-badge');
      pBadge.style.display = 'inline-block';
      pBadge.textContent = `Pruned from ${totalRawCount.toLocaleString()} total`;
    }

    // Tab View Switching
    let currentView = 'tree';
    function switchView(viewName) {
      currentView = viewName;
      document.querySelectorAll('.tab-btn').forEach(b => b.classList.remove('active'));
      document.querySelectorAll('.view-container').forEach(c => c.classList.remove('active'));

      document.getElementById(`tab-btn-${viewName}`).classList.add('active');
      document.getElementById(`view-${viewName}`).classList.add('active');

      if (viewName === 'timeline') {
        renderScatterPlot();
      } else if (viewName === 'leaderboard') {
        renderLeaderboard();
      } else if (viewName === 'tree') {
        if (network) network.fit();
      }
    }

    // Populate dropdowns
    const selectA = document.getElementById('diff-select-a');
    const selectB = document.getElementById('diff-select-b');
    
    // Sort dropdown by occurrences descending so top species are first
    const sortedForDropdown = [...speciesList].sort((a, b) => (b.occurence || 1) - (a.occurence || 1));
    sortedForDropdown.forEach(s => {
      const optA = document.createElement('option');
      optA.value = s.handle;
      optA.textContent = `#${s.handle} (L=${s.genome_length}, Occ=${s.occurence.toLocaleString()})`;
      selectA.appendChild(optA);

      const optB = document.createElement('option');
      optB.value = s.handle;
      optB.textContent = `#${s.handle} (L=${s.genome_length}, Occ=${s.occurence.toLocaleString()})`;
      selectB.appendChild(optB);
    });

    // Subtree collapse tracking (set of collapsed handle IDs)
    const collapsedNodes = new Set();

    // Graph Data Builder & Filter
    let currentGraphData = null;

    function buildGraphData(filterValue, isPhysics = false) {
      let filteredSpecies = [];
      const keepTrunk = document.getElementById('check-keep-trunk').checked;

      // Handle top-N vs threshold filters
      if (filterValue.startsWith('top')) {
        const topN = parseInt(filterValue.replace('top', ''), 10);
        const sorted = [...speciesList].sort((a, b) => (b.occurence || 1) - (a.occurence || 1));
        filteredSpecies = sorted.slice(0, topN);
      } else {
        const minOcc = parseInt(filterValue, 10);
        filteredSpecies = speciesList.filter(s => (s.occurence || 1) >= minOcc);
      }

      // Always include root #0 if it exists
      if (speciesMap.has(0) && !filteredSpecies.some(s => s.handle === 0)) {
        filteredSpecies.push(speciesMap.get(0));
      }

      // Lineage Preservation: Add intermediate ancestors so edges aren't severed
      if (keepTrunk) {
        const handleSet = new Set(filteredSpecies.map(s => s.handle));
        const toAdd = [];
        filteredSpecies.forEach(s => {
          if (s.dna_pre && s.dna_pre.length > 1) {
            for (let i = 1; i < s.dna_pre.length; ++i) {
              const anc = s.dna_pre[i];
              if (speciesMap.has(anc) && !handleSet.has(anc)) {
                handleSet.add(anc);
                toAdd.push(speciesMap.get(anc));
              }
            }
          }
        });
        filteredSpecies = filteredSpecies.concat(toAdd);
      }

      // Build parent-to-children mapping to handle subtree collapse
      const childMap = new Map();
      filteredSpecies.forEach(s => {
        const dnaPre = s.dna_pre || [];
        const p = dnaPre.length > 1 ? dnaPre[1] : s.parent;
        if (!childMap.has(p)) childMap.set(p, []);
        childMap.get(p).push(s.handle);
      });

      // Filter out collapsed descendants
      if (collapsedNodes.size > 0) {
        const hiddenHandles = new Set();
        function markDescendantsHidden(h) {
          const kids = childMap.get(h) || [];
          kids.forEach(kid => {
            hiddenHandles.add(kid);
            markDescendantsHidden(kid);
          });
        }
        collapsedNodes.forEach(h => markDescendantsHidden(h));
        filteredSpecies = filteredSpecies.filter(s => !hiddenHandles.has(s.handle));
      }

      const filteredMap = new Map();
      filteredSpecies.forEach(s => filteredMap.set(s.handle, s));

      const rootSp = speciesMap.get(0);
      const rootLen = rootSp ? rootSp.genome_length : 80;
      const maxOcc = Math.max(...speciesList.map(s => s.occurence || 1), 1);

      const nodes = [];
      const edges = [];

      filteredSpecies.forEach(s => {
        const target = s.handle;
        const norm = Math.log2((s.occurence || 1) + 1) / Math.max(1, Math.log2(maxOcc + 1));
        const size = target === 0 ? 22 : Math.round(9 + norm * 23);

        let colorBg = '#1f6feb', colorBorder = '#58a6ff';
        if (target === 0) {
          colorBg = '#238636'; colorBorder = '#3fb950';
        } else if (s.genome_length < rootLen) {
          colorBg = '#8957e5'; colorBorder = '#bc8cff'; // Parasite
        } else if (s.genome_length > rootLen) {
          colorBg = '#d29922'; colorBorder = '#e3b341'; // Expanded
        }

        const isCollapsed = collapsedNodes.has(target);
        const hasChildren = childMap.has(target) && childMap.get(target).length > 0;
        const labelText = isCollapsed ? `#${target} (+)` : `#${target}`;

        nodes.push({
          id: target,
          label: labelText,
          title: `Species #${target}\nLength: ${s.genome_length} opcodes\nOccurrences: ${(s.occurence || 1).toLocaleString()}\nDOB: Epoch ${s.dob}${isCollapsed ? '\n[Sub-tree Collapsed]' : ''}`,
          size: size,
          shape: 'dot',
          color: { background: colorBg, border: colorBorder, highlight: { background: '#ffffff', border: '#58a6ff' } },
          font: { color: '#c9d1d9', size: Math.max(10, Math.min(13, Math.round(size * 0.65))), face: 'monospace', vadjust: 2 },
          borderWidth: isCollapsed ? 4 : (target === 0 ? 3 : 2),
          borderWidthSelected: 4
        });

        // Edge resolution
        if (target !== 0) {
          let p = 0;
          let intermediateCount = 0;

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
              width: Math.min(3.5, 1.2 + Math.log2((s.occurence || 1) + 1) * 0.35),
              selectionWidth: 3
            };

            if (isPhysics) {
              const driftHops = intermediateCount > 0 ? intermediateCount + 1 : 1;
              const springLen = Math.min(180, 50 + driftHops * 15);
              edgeObj.length = springLen;
              edgeObj.springLength = springLen;
              edgeObj.springConstant = Math.min(0.12, 0.04 + Math.log2((s.occurence || 1) + 1) * 0.015);
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
          layout: { hierarchical: { enabled: false } },
          physics: {
            enabled: true,
            solver: 'repulsion',
            repulsion: { nodeDistance: 85, centralGravity: 0.08, springLength: 70, springConstant: 0.08, damping: 0.35 },
            maxVelocity: 50,
            minVelocity: 0.75,
            timestep: 0.35,
            stabilization: { enabled: true, iterations: 180, updateInterval: 30 }
          },
          interaction: { hover: true, tooltipDelay: 100, selectConnectedEdges: false }
        };
      } else if (layoutMode === 'forceAtlas2') {
        return {
          layout: { hierarchical: { enabled: false } },
          physics: {
            enabled: true,
            solver: 'forceAtlas2Based',
            forceAtlas2Based: { gravitationalConstant: -38, centralGravity: 0.01, springLength: 60, springConstant: 0.08, damping: 0.4 },
            stabilization: { enabled: true, iterations: 180, updateInterval: 30 }
          },
          interaction: { hover: true, tooltipDelay: 100, selectConnectedEdges: false }
        };
      } else {
        return {
          layout: {
            hierarchical: {
              enabled: true,
              direction: layoutMode,
              sortMethod: 'directed',
              levelSeparation: 95,
              nodeSpacing: 65,
              treeSpacing: 100
            }
          },
          physics: { enabled: false },
          interaction: { hover: true, tooltipDelay: 100, selectConnectedEdges: false }
        };
      }
    }

    // Initialize Network
    const container = document.getElementById('network-container');
    currentGraphData = buildGraphData('top100', false);
    const initialOptions = getNetworkOptions('LR');
    const network = new vis.Network(container, currentGraphData, initialOptions);

    document.getElementById('visible-count-badge').textContent = `${currentGraphData.total} / ${speciesList.length}`;

    network.on('stabilizationIterationsDone', function () {
      const mode = document.getElementById('layout-select').value;
      if (mode === 'spring' || mode === 'forceAtlas2') {
        network.setOptions({ physics: { enabled: false } });
      }
    });

    function rebuildGraph() {
      const mode = document.getElementById('layout-select').value;
      const isPhysics = (mode === 'spring' || mode === 'forceAtlas2');
      const filterValue = document.getElementById('filter-occ').value;

      currentGraphData = buildGraphData(filterValue, isPhysics);
      const rawNodes = currentGraphData.nodes.get();
      const cleanNodes = isPhysics ? rawNodes.map(n => {
        const copy = Object.assign({}, n);
        delete copy.x; delete copy.y;
        return copy;
      }) : rawNodes;

      network.setData({ nodes: new vis.DataSet(cleanNodes), edges: currentGraphData.edges });
      document.getElementById('visible-count-badge').textContent = `${currentGraphData.total} / ${speciesList.length}`;

      if (isPhysics) {
        network.setOptions(getNetworkOptions(mode));
        network.stabilize(180);
      } else {
        setTimeout(() => { network.fit(); }, 120);
      }
    }

    function changeLayout() {
      const mode = document.getElementById('layout-select').value;
      const isPhysics = (mode === 'spring' || mode === 'forceAtlas2');
      const filterValue = document.getElementById('filter-occ').value;

      network.setOptions(getNetworkOptions(mode));
      currentGraphData = buildGraphData(filterValue, isPhysics);

      const rawNodes = currentGraphData.nodes.get();
      const cleanNodes = isPhysics ? rawNodes.map(n => {
        const copy = Object.assign({}, n);
        delete copy.x; delete copy.y;
        return copy;
      }) : rawNodes;

      network.setData({ nodes: new vis.DataSet(cleanNodes), edges: currentGraphData.edges });

      if (isPhysics) {
        network.stabilize(180);
      }
      setTimeout(() => { network.fit(); }, 200);
    }

    function resetZoom() {
      if (network) network.fit({ animation: { duration: 350, easingFunction: 'easeInOutQuad' } });
    }

    function focusSpecies(handleId) {
      if (network && currentGraphData) {
        // If node is currently filtered out, switch to all and find it
        if (!currentGraphData.nodes.get(handleId)) {
          document.getElementById('filter-occ').value = '0';
          rebuildGraph();
        }
        network.selectNodes([handleId]);
        network.focus(handleId, { scale: 1.15, animation: { duration: 350, easingFunction: 'easeInOutQuad' } });
      }
    }

    function searchSpecies() {
      const val = parseInt(document.getElementById('search-id-input').value, 10);
      if (isNaN(val)) return;
      if (speciesMap.has(val)) {
        if (currentView !== 'tree') switchView('tree');
        focusSpecies(val);
        inspectSpecies(val);
      } else {
        alert(`Species #${val} not found in this dataset.`);
      }
    }

    // Toggle Subtree Collapse on Node Double-click
    network.on('doubleClick', function(params) {
      if (params.nodes.length > 0) {
        const nodeId = params.nodes[0];
        toggleSubtree(nodeId);
      }
    });

    function toggleSubtree(handleId) {
      if (collapsedNodes.has(handleId)) {
        collapsedNodes.delete(handleId);
      } else {
        collapsedNodes.add(handleId);
      }
      rebuildGraph();
      inspectSpecies(handleId);
    }

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
          <button class="btn" style="padding: 4px 8px; font-size: 0.78rem;" onclick="inspectSpecies(${fromId}, false)">Inspect #${fromId}</button>
          <button class="btn" style="padding: 4px 8px; font-size: 0.78rem;" onclick="inspectSpecies(${toId}, false)">Inspect #${toId}</button>
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
      const parent = dnaPre.length > 1 ? dnaPre[1] : s.parent;
      const driftHops = dnaPre.length > 1 ? dnaPre.length - 1 : 1;
      const isCollapsed = collapsedNodes.has(handleId);

      let html = `
        <table class="meta-table">
          <tr><td>Species Handle</td><td><strong>#${s.handle}</strong></td></tr>
          <tr><td>Parent Species</td><td><strong>#${parent}</strong></td></tr>
          <tr><td>Genome Length</td><td>${s.genome_length} opcodes</td></tr>
          <tr><td>Occurrences</td><td>${(s.occurence || 1).toLocaleString()} replicates</td></tr>
          <tr><td>Date of Birth</td><td>Epoch ${s.dob}</td></tr>
          <tr><td>Mutational Drift</td><td>${driftHops} intermediate step(s)</td></tr>
        </table>
        <div style="display: flex; gap: 8px; margin-top: 8px; flex-wrap: wrap;">
          <button class="btn" style="padding: 4px 8px; font-size: 0.78rem;" onclick="setAsA(${s.handle})">Set as Diff A</button>
          <button class="btn" style="padding: 4px 8px; font-size: 0.78rem;" onclick="setAsB(${s.handle})">Set as Diff B</button>
          <button class="btn" style="padding: 4px 8px; font-size: 0.78rem;" onclick="compareWithFounder(${s.handle})">Diff vs Founder</button>
          <button class="btn" style="padding: 4px 8px; font-size: 0.78rem;" onclick="toggleSubtree(${s.handle})">
            ${isCollapsed ? '➕ Expand Sub-tree' : '➖ Collapse Sub-tree'}
          </button>
        </div>
      `;

      document.getElementById('sidebar-content').innerHTML = html;

      if (autoDiff) {
        if (selectA.value == handleId) {
          setAsB(parent);
        } else {
          setAsB(handleId);
        }
      }
    }

    function setAsA(id) { selectA.value = id; focusSpecies(id); renderDiff(); }
    function setAsB(id) { selectB.value = id; focusSpecies(id); renderDiff(); }
    function setDiffPair(idA, idB) { selectA.value = idA; selectB.value = idB; renderDiff(); }
    function compareWithFounder(id) { setDiffPair(0, id); }

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

    // ==========================================
    // VIEW 2: Genome Evolution Timeline (Scatter Canvas)
    // ==========================================
    let scatterPoints = [];
    const canvas = document.getElementById('timeline-canvas');
    const ctx = canvas.getContext('2d');
    const tooltip = document.getElementById('scatter-tooltip');

    function renderScatterPlot() {
      const wrapper = document.getElementById('canvas-wrapper');
      canvas.width = wrapper.clientWidth;
      canvas.height = wrapper.clientHeight;

      if (speciesList.length === 0) return;

      const padLeft = 60, padRight = 30, padTop = 30, padBottom = 50;
      const w = canvas.width - padLeft - padRight;
      const h = canvas.height - padTop - padBottom;

      const maxDob = Math.max(...speciesList.map(s => s.dob || 0), 100);
      const minLen = Math.min(...speciesList.map(s => s.genome_length || 0), 20);
      const maxLen = Math.max(...speciesList.map(s => s.genome_length || 0), 80);
      const maxOcc = Math.max(...speciesList.map(s => s.occurence || 1), 1);
      const rootLen = speciesMap.get(0) ? speciesMap.get(0).genome_length : 80;

      // Clear Canvas
      ctx.fillStyle = '#090d13';
      ctx.fillRect(0, 0, canvas.width, canvas.height);

      // Grid lines
      ctx.strokeStyle = '#21262d';
      ctx.lineWidth = 1;
      ctx.fillStyle = '#8b949e';
      ctx.font = '10px monospace';

      // X-axis (Epochs)
      for (let i = 0; i <= 5; ++i) {
        const epoch = Math.round((maxDob / 5) * i);
        const x = padLeft + (epoch / maxDob) * w;
        ctx.beginPath();
        ctx.moveTo(x, padTop);
        ctx.lineTo(x, padTop + h);
        ctx.stroke();
        ctx.fillText(`Epoch ${epoch}`, x - 25, padTop + h + 20);
      }

      // Y-axis (Length)
      const lenStep = Math.max(5, Math.round((maxLen - minLen) / 5));
      for (let len = minLen; len <= maxLen; len += lenStep) {
        const y = padTop + h - ((len - minLen) / (maxLen - minLen)) * h;
        ctx.beginPath();
        ctx.moveTo(padLeft, y);
        ctx.lineTo(padLeft + w, y);
        ctx.stroke();
        ctx.fillText(`${len} ops`, 15, y + 4);
      }

      // Axis labels
      ctx.fillStyle = '#c9d1d9';
      ctx.font = '11px sans-serif';
      ctx.fillText('Genome Length (Opcodes) →', 10, padTop - 12);
      ctx.fillText('Evolutionary Time (Date of Birth) →', padLeft + w / 2 - 90, padTop + h + 38);

      // Plot Bubbles
      scatterPoints = [];
      speciesList.forEach(s => {
        const x = padLeft + ((s.dob || 0) / maxDob) * w;
        const y = padTop + h - (((s.genome_length || 0) - minLen) / Math.max(1, maxLen - minLen)) * h;
        const norm = Math.log2((s.occurence || 1) + 1) / Math.max(1, Math.log2(maxOcc + 1));
        const r = s.handle === 0 ? 8 : Math.max(3.5, Math.min(22, 3.5 + norm * 18));

        let color = '#1f6feb';
        if (s.handle === 0) color = '#238636';
        else if (s.genome_length < rootLen) color = '#8957e5';
        else if (s.genome_length > rootLen) color = '#d29922';

        ctx.beginPath();
        ctx.arc(x, y, r, 0, Math.PI * 2);
        ctx.fillStyle = color;
        ctx.globalAlpha = 0.75;
        ctx.fill();
        ctx.globalAlpha = 1.0;
        ctx.strokeStyle = '#ffffff';
        ctx.lineWidth = s.handle === 0 ? 2 : 0.75;
        ctx.stroke();

        scatterPoints.push({ x, y, r, species: s });
      });
    }

    // Canvas Hover & Click Interaction
    canvas.addEventListener('mousemove', (e) => {
      const rect = canvas.getBoundingClientRect();
      const mx = e.clientX - rect.left;
      const my = e.clientY - rect.top;

      let found = null;
      for (let i = scatterPoints.length - 1; i >= 0; --i) {
        const p = scatterPoints[i];
        const dist = Math.hypot(p.x - mx, p.y - my);
        if (dist <= p.r + 3) {
          found = p.species;
          break;
        }
      }

      if (found) {
        tooltip.style.display = 'block';
        tooltip.style.left = `${mx + 15}px`;
        tooltip.style.top = `${my - 15}px`;
        tooltip.innerHTML = `
          <strong>Species #${found.handle}</strong><br>
          Length: ${found.genome_length} opcodes<br>
          Replications: ${(found.occurence || 1).toLocaleString()}<br>
          Born: Epoch ${found.dob}<br>
          Parent: #${found.parent}<br>
          <em style="color: var(--accent); font-size: 0.72rem;">Click to view code diff</em>
        `;
        document.getElementById('scatter-status').textContent = `Species #${found.handle} (${found.genome_length} ops, ${(found.occurence||1).toLocaleString()} reps)`;
      } else {
        tooltip.style.display = 'none';
        document.getElementById('scatter-status').textContent = 'Hover over a point for details';
      }
    });

    canvas.addEventListener('click', (e) => {
      const rect = canvas.getBoundingClientRect();
      const mx = e.clientX - rect.left;
      const my = e.clientY - rect.top;

      for (let i = scatterPoints.length - 1; i >= 0; --i) {
        const p = scatterPoints[i];
        if (Math.hypot(p.x - mx, p.y - my) <= p.r + 3) {
          switchView('tree');
          focusSpecies(p.species.handle);
          inspectSpecies(p.species.handle);
          break;
        }
      }
    });

    window.addEventListener('resize', () => {
      if (currentView === 'timeline') renderScatterPlot();
    });

    // ==========================================
    // VIEW 3: Species Leaderboard Table
    // ==========================================
    function renderLeaderboard() {
      const tbody = document.getElementById('leaderboard-body');
      tbody.innerHTML = '';

      const totalReps = speciesList.reduce((acc, s) => acc + (s.occurence || 1), 0);
      const topList = [...speciesList].sort((a, b) => (b.occurence || 1) - (a.occurence || 1)).slice(0, 50);

      document.getElementById('leaderboard-count-badge').textContent = `Top ${topList.length} of ${speciesList.length} Species`;

      topList.forEach((s, idx) => {
        const tr = document.createElement('tr');
        const share = ((s.occurence || 1) / Math.max(1, totalReps)) * 100;
        const rootLen = speciesMap.get(0) ? speciesMap.get(0).genome_length : 80;

        let lenBadgeColor = '#1f6feb';
        if (s.handle === 0) lenBadgeColor = '#238636';
        else if (s.genome_length < rootLen) lenBadgeColor = '#8957e5';
        else if (s.genome_length > rootLen) lenBadgeColor = '#d29922';

        tr.innerHTML = `
          <td><strong>#${idx + 1}</strong></td>
          <td><strong style="color: var(--accent);">#${s.handle}</strong></td>
          <td><span class="badge" style="color: ${lenBadgeColor}; border-color: ${lenBadgeColor};">${s.genome_length} opcodes</span></td>
          <td>
            <div class="bar-cell">
              <span style="font-weight: 600; width: 85px;">${(s.occurence || 1).toLocaleString()}</span>
              <div class="bar-fill" style="width: ${Math.max(4, Math.min(180, share * 3.5))}px;"></div>
              <span style="color: var(--text-muted); font-size: 0.75rem;">${share.toFixed(1)}%</span>
            </div>
          </td>
          <td>Epoch ${s.dob}</td>
          <td>#${s.parent}</td>
          <td>
            <button class="btn" style="padding: 2px 8px; font-size: 0.74rem;" onclick="jumpToSpeciesFromLeaderboard(${s.handle})">Diff vs Founder</button>
          </td>
        `;
        tbody.appendChild(tr);
      });
    }

    function jumpToSpeciesFromLeaderboard(handleId) {
      switchView('tree');
      focusSpecies(handleId);
      inspectSpecies(handleId);
      setDiffPair(0, handleId);
    }

    // Default select first two species
    if (speciesList.length >= 2) {
      selectA.value = 0;
      selectB.value = sortedForDropdown[0].handle === 0 ? sortedForDropdown[1].handle : sortedForDropdown[0].handle;
      inspectSpecies(parseInt(selectB.value, 10), false);
      renderDiff();
    } else if (speciesList.length === 1) {
      inspectSpecies(speciesList[0].handle, false);
    }
  </script>
</body>
</html>
"""

def prune_and_filter_evodex(data, top_n=None, min_occ=None, keep_all=False):
    """
    Intelligently prune large EvoDex datasets:
    - Selects the most dominant species (by top-N or min occurrence).
    - Preserves all ancestral bridge nodes back to root Founder (#0) so the tree is 100% connected.
    - Slashes payload size by 98%+ while preserving full macro-evolutionary integrity.
    """
    species = data.get("evodex", [])
    if not species or keep_all:
        return data

    total_raw = len(species)
    sp_map = {s["handle"]: s for s in species}

    # Decide candidate targets
    if min_occ is not None:
        candidates = {s["handle"] for s in species if s.get("occurence", 1) >= min_occ}
    elif top_n is not None:
        sorted_sp = sorted(species, key=lambda s: s.get("occurence", 1), reverse=True)
        candidates = {s["handle"] for s in sorted_sp[:top_n]}
    else:
        # Default auto-pruning if dataset is massive (> 300 species)
        if total_raw > 300:
            top_n = 200
            sorted_sp = sorted(species, key=lambda s: s.get("occurence", 1), reverse=True)
            candidates = {s["handle"] for s in sorted_sp[:top_n]}
        else:
            return data

    # Always ensure root #0 is present
    if 0 in sp_map:
        candidates.add(0)

    # Lineage Preservation: Add all intermediate ancestors along dna_pre and parent chains
    lineage_handles = set(candidates)
    for h in candidates:
        s = sp_map.get(h)
        if not s:
            continue

        # Traverse dna_pre
        for anc in s.get("dna_pre", []):
            if anc in sp_map:
                lineage_handles.add(anc)

        # Traverse parent chain
        curr = s.get("parent", 0)
        while curr != 0 and curr in sp_map:
            lineage_handles.add(curr)
            p_obj = sp_map.get(curr)
            if not p_obj or p_obj.get("parent") == curr:
                break
            curr = p_obj.get("parent", 0)

    pruned_species = [sp_map[h] for h in lineage_handles if h in sp_map]

    # Return trimmed copy
    pruned_data = {
        "evodex": pruned_species,
        "memory_size": data.get("memory_size", 0),
        "total_raw_count": total_raw
    }
    return pruned_data

def main():
    parser = argparse.ArgumentParser(description="Export EvoDex JSON into an interactive Standalone HTML Genome Explorer.")
    parser.add_argument("json_file", help="Path to evodex.json")
    parser.add_argument("-o", "--out", default="evodex_explorer.html", help="Output HTML file path (default: evodex_explorer.html)")
    parser.add_argument("-t", "--top", type=int, default=None, help="Keep top N most replicated species + their ancestral lineages")
    parser.add_argument("-m", "--min-occ", type=int, default=None, help="Filter species with >= M occurrences + their ancestral lineages")
    parser.add_argument("--all", action="store_true", help="Export all species without pruning (warning: large files may lag browser)")
    parser.add_argument("--no-browser", action="store_true", help="Do not automatically open the browser")

    args = parser.parse_args()

    if not os.path.exists(args.json_file):
        print(f"Error: JSON file not found: {args.json_file}")
        sys.exit(1)

    print(f"Loading {args.json_file}...")
    with open(args.json_file, "r") as f:
        raw_data = json.load(f)

    raw_count = len(raw_data.get("evodex", []))

    # Apply smart pruning
    data = prune_and_filter_evodex(raw_data, top_n=args.top, min_occ=args.min_occ, keep_all=args.all)
    pruned_count = len(data.get("evodex", []))

    if pruned_count < raw_count:
        print(f"Intelligent Pruning Applied: Retained {pruned_count} dominant species + ancestral lineages (from {raw_count:,} total).")
    else:
        print(f"Exporting all {pruned_count:,} species.")

    json_str = json.dumps(data)
    html_content = HTML_TEMPLATE.replace("__EMBEDDED_JSON_DATA__", json_str)

    with open(args.out, "w", encoding="utf-8") as f:
        f.write(html_content)

    file_size_kb = os.path.getsize(args.out) / 1024
    if file_size_kb >= 1024:
        size_str = f"{file_size_kb / 1024:.2f} MB"
    else:
        size_str = f"{file_size_kb:.1f} KB"

    print(f"Successfully generated Genome Explorer ({size_str}) at: {args.out}")
    if not args.no_browser:
        webbrowser.open("file://" + os.path.abspath(args.out))

if __name__ == "__main__":
    main()

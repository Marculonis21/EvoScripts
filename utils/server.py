#!/usr/bin/env python3
import sys
import os
import json
import argparse
import webbrowser
from urllib.parse import urlparse, parse_qs
from http.server import HTTPServer, SimpleHTTPRequestHandler

# Import template and pruning logic from export_html
sys.path.insert(0, os.path.dirname(__file__))
from export_html import HTML_TEMPLATE, prune_and_filter_evodex

class EvoDexLiveHandler(SimpleHTTPRequestHandler):
    json_path = "evodex.json"
    top_n = None
    min_occ = None
    keep_all = False

    def do_GET(self):
        parsed_url = urlparse(self.path)
        query = parse_qs(parsed_url.query)

        # Allow query string overrides: /?top=100 or /api/data?min_occ=50
        req_top = int(query["top"][0]) if "top" in query else self.top_n
        req_min_occ = int(query["min_occ"][0]) if "min_occ" in query else self.min_occ
        req_all = ("all" in query) or self.keep_all

        if parsed_url.path == "/" or parsed_url.path.startswith("/index.html"):
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.end_headers()

            # Serve HTML template with live reload script injected
            raw_data = self.read_json()
            data = prune_and_filter_evodex(raw_data, top_n=req_top, min_occ=req_min_occ, keep_all=req_all)
            json_str = json.dumps(data)
            html = HTML_TEMPLATE.replace("__EMBEDDED_JSON_DATA__", json_str)
            
            # Add a live-refresh button to header
            refresh_snippet = """
            <button class="btn btn-primary" style="padding: 4px 10px;" onclick="reloadLiveData()">🔄 Live Reload</button>
            <script>
              async function reloadLiveData() {
                try {
                  const res = await fetch('/api/data' + window.location.search);
                  const freshData = await res.json();
                  location.reload();
                } catch(e) {
                  alert('Error loading live data: ' + e);
                }
              }
            </script>
            """
            html = html.replace('</div>\n  </header>', f'{refresh_snippet}</div>\n  </header>')
            self.wfile.write(html.encode("utf-8"))

        elif parsed_url.path == "/api/data":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.send_header("Cache-Control", "no-cache, no-store, must-revalidate")
            self.end_headers()

            raw_data = self.read_json()
            data = prune_and_filter_evodex(raw_data, top_n=req_top, min_occ=req_min_occ, keep_all=req_all)
            self.wfile.write(json.dumps(data).encode("utf-8"))
        else:
            super().do_GET()

    def read_json(self):
        if not os.path.exists(self.json_path):
            return {"evodex": [], "memory": []}
        try:
            with open(self.json_path, "r") as f:
                return json.load(f)
        except Exception:
            return {"evodex": [], "memory": []}

def run_server(json_path, port=8000, top_n=None, min_occ=None, keep_all=False, no_browser=False):
    EvoDexLiveHandler.json_path = json_path
    EvoDexLiveHandler.top_n = top_n
    EvoDexLiveHandler.min_occ = min_occ
    EvoDexLiveHandler.keep_all = keep_all

    server_address = ("", port)
    httpd = HTTPServer(server_address, EvoDexLiveHandler)
    url = f"http://localhost:{port}/"
    print(f"==================================================")
    print(f"🚀 EvoScripts Live Explorer running at: {url}")
    print(f"📂 Tracking data from: {json_path}")
    if top_n:
        print(f"🎯 Default View: Top {top_n} dominant clades + ancestral lineages")
    elif min_occ:
        print(f"🎯 Default View: Occurrences >= {min_occ}")
    print(f"   Press Ctrl+C to stop the server.")
    print(f"==================================================")

    if not no_browser:
        webbrowser.open(url)

    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nStopping server...")
        httpd.server_close()

def main():
    parser = argparse.ArgumentParser(description="Live Web Server for EvoScripts Genome Explorer.")
    parser.add_argument("json_file", nargs="?", default="evodex.json", help="Path to evodex.json (default: evodex.json)")
    parser.add_argument("-p", "--port", type=int, default=8000, help="Port to serve on (default: 8000)")
    parser.add_argument("-t", "--top", type=int, default=None, help="Keep top N most replicated species + ancestral lineages")
    parser.add_argument("-m", "--min-occ", type=int, default=None, help="Filter species with >= M occurrences + ancestral lineages")
    parser.add_argument("--all", action="store_true", help="Serve all species without pruning")
    parser.add_argument("--no-browser", action="store_true", help="Do not open browser automatically")

    args = parser.parse_args()
    run_server(args.json_file, args.port, top_n=args.top, min_occ=args.min_occ, keep_all=args.all, no_browser=args.no_browser)

if __name__ == "__main__":
    main()

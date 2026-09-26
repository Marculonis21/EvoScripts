#!/usr/bin/env python3
import sys
import os
import json
import argparse
import webbrowser
from http.server import HTTPServer, SimpleHTTPRequestHandler

# Import template from export_html
sys.path.insert(0, os.path.dirname(__file__))
from export_html import HTML_TEMPLATE

class EvoDexLiveHandler(SimpleHTTPRequestHandler):
    json_path = "evodex.json"

    def do_GET(self):
        if self.path == "/" or self.path.startswith("/index.html"):
            self.send_response(200)
            self.send_header("Content-Type", "text/html; charset=utf-8")
            self.end_headers()

            # Serve HTML template with live reload script injected
            data = self.read_json()
            json_str = json.dumps(data)
            html = HTML_TEMPLATE.replace("__EMBEDDED_JSON_DATA__", json_str)
            
            # Add a live-refresh button to header
            refresh_snippet = """
            <button class="btn" style="width: auto; margin-top: 0; padding: 4px 10px; background: #238636; border-color: #3fb950;" onclick="reloadLiveData()">🔄 Live Reload</button>
            <script>
              async function reloadLiveData() {
                try {
                  const res = await fetch('/api/data');
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

        elif self.path == "/api/data":
            self.send_response(200)
            self.send_header("Content-Type", "application/json")
            self.send_header("Cache-Control", "no-cache, no-store, must-revalidate")
            self.end_headers()

            data = self.read_json()
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

def run_server(json_path, port=8000, no_browser=False):
    EvoDexLiveHandler.json_path = json_path
    server_address = ("", port)
    httpd = HTTPServer(server_address, EvoDexLiveHandler)
    url = f"http://localhost:{port}/"
    print(f"==================================================")
    print(f"🚀 EvoScripts Live Explorer running at: {url}")
    print(f"📂 Tracking data from: {json_path}")
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
    parser.add_argument("--no-browser", action="store_true", help="Do not open browser automatically")

    args = parser.parse_args()
    run_server(args.json_file, args.port, args.no_browser)

if __name__ == "__main__":
    main()

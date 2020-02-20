# OpenStreetMap Live View
Used to plot GPS path from KML file and monitor GPS data from `/agribot/gnss`

Data is output to HTML file refreshed every 5 seconds

---

#### Install requirements:
`sudo pip install -r requirements.txt`

---

#### Workflow:
1. Run `run_live_view.sh /path/to/file/example.kml`
2. Open *_map.html* (from *agribot_commander/scripts/aux/live_view*) file in browser (may require refreshing HTML by hand) or in Atom/VisualStudioCode (both should have package for HTML preview with autorefresh)

---

Refresh time (rospy.Rate) and number of measurements to ignore between adding GPS point to list can be adjusted in *map_vis_live.ipynb* notebook

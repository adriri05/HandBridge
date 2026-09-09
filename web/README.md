# HandBridge — Web

React frontend + Node/Express backend for viewing live gesture output and (optionally) connecting to the glove from the browser via Web Bluetooth.

## Architecture

- `backend/` — Express server. Exposes REST for config/history, WebSocket for live decoded-gesture stream. Can receive glove data relayed from the mobile app, or (future) bridge Web Bluetooth connections itself.
- `frontend/` — React app. Connects to backend over WebSocket, displays live decoded text, gesture history.

## Setup

```
cd web/backend
npm install
npm run dev       # starts Express + WS server

cd web/frontend
npm install
npm run dev       # starts React dev server
```

## TODO

- [ ] Define REST/WS API contract between frontend and backend
- [ ] Implement gesture decoding on backend per docs/gesture-protocol.md
- [ ] Web Bluetooth direct-connect option (optional, browser support permitting)
- [ ] Live gesture display + history view in frontend
- [ ] Auth (if multi-user support needed)

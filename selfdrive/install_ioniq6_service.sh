#!/usr/bin/bash
# Install Ioniq 6 ECU Disable Service

set -e

echo "Installing Ioniq 6 ECU Disable Service..."

# Make the Python script executable
chmod +x /data/openpilot/selfdrive/ioniq6_ecu_disable_service.py

# Copy service file to systemd directory
sudo cp /data/openpilot/selfdrive/ioniq6-ecu-disable.service /etc/systemd/system/

# Reload systemd to recognize new service
sudo systemctl daemon-reload

# Enable service to run at boot
sudo systemctl enable ioniq6-ecu-disable.service

echo "✓ Service installed and enabled"
echo ""
echo "To start the service now (test it):"
echo "  sudo systemctl start ioniq6-ecu-disable.service"
echo ""
echo "To check service status:"
echo "  sudo systemctl status ioniq6-ecu-disable.service"
echo ""
echo "To view service logs:"
echo "  journalctl -u ioniq6-ecu-disable.service -f"
echo ""
echo "To disable the service:"
echo "  sudo systemctl disable ioniq6-ecu-disable.service"

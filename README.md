
# graphinator

Simple widget for the XFCE desktop panel showing usage graphs for CPU and
memory.

![Image demonstration](res/image.png)


## Installation

```sh
# Prerequisites:
sudo apt update
sudo apt install xfce4-dev-tools \
    libxfce4ui-2-dev \
    libxfce4panel-2.0-dev \
    imagemagick \
    libgtk-3-dev

# Download:
git clone https://github.com/Bleskocvok/graphinator.git

# Build:
cd graphinator
sh ./configure
make
sudo bash script/install
```

## Activation

Right click on the XFCE panel → Panel → Add New Items… → Select `Graphinator`

## List of TODOs

- Show net usage (with sensible units, percentage makes little sense)
- Allow for custom monitoring (like `xfce4-genmon-plugin`)
- Settings for text orientation
- Show grid option
- Show background option
- Show ruler option
- Options to show all cpu cores
- ~~Simple bar graph rendering option~~

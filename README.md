# Goalie

A simple Pebble Time app that lets you set a daily health goal and get notified with vibrations and 
a popup when you reach it.

Goalie runs in the background so you can get notified even if you're not in the app.

Supports Pebble Time, Pebble Time Steel, and Pebble Time Round watches.  

![Basalt progress screenshot](images/basalt_progress.gif)
![Basalt goal screenshot](images/basalt_goal.gif)

![Chalk progress screenshot](images/chalk_progress.gif)
![Chalk goal screenshot](images/chalk_goal.gif)

The app includes configuration options for:
* Goal type
    * Steps
    * Distance (meters)
* Goal value
* Popup timeout

## Building on Mac OSX

Make sure you have the latest version of the Pebble SDK Homebrew package installed:

```
brew update
brew install pebble-sdk
brew upgrade pebble-sdk # if already installed
```

Next install the latest version of the Pebble SDK using the pebble tool:

```
pebble sdk install latest
```

Finally, build the watchface:

```
pebble build
```

And install it into the emulator of the platform of your choice (chalk shown below) with:

```
pebble install --emulator chalk
```

Or install it on your watch after enabling the Developer Connection in your Pebble Time 
iPhone/Android app with:

```
pebble install --phone IP_ADDRESS
```

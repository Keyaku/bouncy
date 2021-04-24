Bouncy - Godot OpenCV integration example
=========================================

An example of using OpenCV in your Godot game through GDNative API. It works with Godot 3.1+, the example allows you to control a ball initialized using face detection using optical flow in the camera (e.g. by waving hand).

Setup
-----

You must have OpenCV installed and discoverable.  
For macOS, you can use Brew (beware of the many dependencies!):  
`brew install opencv`

Then, compile the native code in the `gdnative/src` directory:  
`cd gdnative/src && cmake . && make`

This will produce library `bouncy` and will automatically put it in `gdnative/bouncy`.  
The `gdnlib` file is ready for this setup for Windows, macOS and Linux 64-bit libraries. If you need more, you can edit the file `gdnative/bouncy/bouncy.gdnlib` with your editor of preference, or with Godot directly.



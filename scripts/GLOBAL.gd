extends Node

onready var root = get_tree().get_root()

# Error checking signal connection. Returns Error code just in case
func connect_signal(source: Object, signal_name: String, target: Object, method: String, binds: Array = [], flags: int = 0):
	var err = source.connect(signal_name, target, method, binds, flags)
	if err != 0:
		printerr("Error ", err, ". Signal connection failed.")
	return err

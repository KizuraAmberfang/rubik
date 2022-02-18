extends Spatial

export var upper : Color
export var front : Color
export var right : Color
export var left : Color
export var back : Color
export var down : Color

# Called when the node enters the scene tree for the first time.
func _ready():
	$CentralPieceO.faceColor = front
	$SpigoloOG.faceColorB = front
	$SpigoloOG.faceColorA = right
	$SpigoloBO.faceColorA = front
	$SpigoloBO.faceColorB = left
	
	$CentralPieceO.change()
	$SpigoloOG.change()
	$SpigoloBO.change()

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

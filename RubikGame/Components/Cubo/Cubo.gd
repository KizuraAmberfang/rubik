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
	$CentralPieceG.faceColor = right
	$CentralPieceW.faceColor = upper
	$CentralPieceB.faceColor = left
	$CentralPieceR.faceColor = back
	$CentralPieceY.faceColor = down
	
	$SpigoloOG.faceColorR = front
	$SpigoloOG.faceColorB = right
	$SpigoloBO.faceColorB = front
	$SpigoloBO.faceColorR = left
	$SpigoloOY.faceColorB = front
	$SpigoloOY.faceColorR = down
	$SpigoloWO.faceColorR = front
	$SpigoloWO.faceColorB = upper
	$SpigoloGW.faceColorB = right
	$SpigoloGW.faceColorR = upper
	$SpigoloGY.faceColorB = down
	$SpigoloGY.faceColorR = right
	$SpigoloGR.faceColorB = back
	$SpigoloGR.faceColorR = right
	$SpigoloWR.faceColorB = back
	$SpigoloWR.faceColorR = upper
	$SpigoloWB.faceColorB = left
	$SpigoloWB.faceColorR = upper
	$SpigoloBR.faceColorB = left
	$SpigoloBR.faceColorR = back
	$SpigoloYB.faceColorR = left
	$SpigoloYB.faceColorB = down
	$SpigoloYR.faceColorR = back
	$SpigoloYR.faceColorB = down
	
	$VerticeOWB.faceColorB = upper
	$VerticeOWB.faceColorG = left
	$VerticeOWB.faceColorR = front
	$VerticeWOG.faceColorB = right
	$VerticeWOG.faceColorG = upper
	$VerticeWOG.faceColorR = front
	$VerticeOGY.faceColorB = front
	$VerticeOGY.faceColorG = down
	$VerticeOGY.faceColorR = right
	$VerticeOBY.faceColorB = front
	$VerticeOBY.faceColorG = left
	$VerticeOBY.faceColorR = down
	$VerticeRGY.faceColorB = right
	$VerticeRGY.faceColorG = down
	$VerticeRGY.faceColorR = back
	$VerticeWGR.faceColorB = upper
	$VerticeWGR.faceColorG = right
	$VerticeWGR.faceColorR = back
	$VerticeWBR.faceColorB = upper
	$VerticeWBR.faceColorG = back
	$VerticeWBR.faceColorR = left
	$VerticeRBY.faceColorB = left
	$VerticeRBY.faceColorG = back
	$VerticeRBY.faceColorR = down
	
	$CentralPieceO.change()
	$CentralPieceG.change()
	$CentralPieceW.change()
	$CentralPieceB.change()
	$CentralPieceR.change()
	$CentralPieceY.change()
	
	$SpigoloOG.change()
	$SpigoloBO.change()
	$SpigoloOY.change()
	$SpigoloWO.change()
	$SpigoloGW.change()
	$SpigoloGY.change()
	$SpigoloGR.change()
	$SpigoloWR.change()
	$SpigoloWB.change()
	$SpigoloBR.change()
	$SpigoloYB.change()
	$SpigoloYR.change()
	
	$VerticeOWB.change()
	$VerticeWOG.change()
	$VerticeOGY.change()
	$VerticeOBY.change()
	$VerticeRGY.change()
	$VerticeWGR.change()
	$VerticeWBR.change()
	$VerticeRBY.change()
# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

extends RigidBody

onready var faceColorG : Color
onready var faceColorR : Color
onready var faceColorB : Color

# Called when the node enters the scene tree for the first time.
func _ready():
	pass # Replace with function body.

func change():
	var matG = SpatialMaterial.new()
	matG.albedo_color = faceColorG
	$FacciaG.set_material_override(matG)
	var matR = SpatialMaterial.new()
	matR.albedo_color = faceColorR
	$FacciaR.set_material_override(matR)
	var matB = SpatialMaterial.new()
	matB.albedo_color = faceColorB
	$FacciaB.set_material_override(matB)
# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

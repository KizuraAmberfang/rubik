extends RigidBody

onready var faceColorB : Color
onready var faceColorR : Color

# Called when the node enters the scene tree for the first time.
func _ready():
	pass

func change():
	var matB = SpatialMaterial.new()
	matB.albedo_color = faceColorB
	$FacciaA.set_material_override(matB)
	var matR = SpatialMaterial.new()
	matR.albedo_color = faceColorR
	$FacciaR.set_material_override(matR)

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

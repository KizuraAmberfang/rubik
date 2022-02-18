extends RigidBody

var faceColor : Color

# Called when the node enters the scene tree for the first time.
func _ready():
	pass

func change():
	var mat = SpatialMaterial.new()
	mat.albedo_color = faceColor
	$Face.set_material_override(mat)

# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass

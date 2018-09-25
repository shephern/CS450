import struct
import bpy
from bpy import context

# 
# Only tested for v2.72, but might work on others
# If you want to use materials, set the engine to Cycles, 
    # and just assign diffuse mats to the polys
# Exports in OpenGL, which then can be pasted into your environment
# Currently only works for objects made entirely of planar quads
    # That will change, just haven't yet

def openGL_exporter(context, filepath, use_some_setting):
    print("running write_some_data...")
    f = open(filepath, 'w', encoding='utf-8')
    objs = context.selected_objects
    if not len(objs) and objs[0].type() == "mesh":
    	print("active object must be a mesh")
    	return

    # Getting the necessary object
    obj = objs[0]
    mesh = obj.data
    	
    f = open(filepath, "wt")
    
    # Quickly link vertice index to their coords
    vertDict = {}
    index = 0
    for vert in mesh.vertices:
        vert_coords = (vert.co[0],vert.co[1],vert.co[2])
        vertDict[index] = vert_coords
        index += 1

    # Get list of material names, to fill with associated polys
    matDict = {ms.material.name : [] for ms in obj.material_slots}
    
    for idx, poly in enumerate( mesh.polygons ):
        matDict[ obj.material_slots[ poly.material_index ].name ].append( idx )
    
    
#	# I don't know how many faces there are
#	# since I will export only triangles I will now count them
	
    numfaces = 0
    for face in mesh.polygons:
        if len(face.vertices) == 4:
            numfaces += 2
        else:
            print("Currently only accepts quads\n")
            return

    

	# Here we write the faces to the text
    for mat in obj.material_slots:
        # Sorted by material, so that the color changes don't have to be repeated
        mat_color = mat.material.diffuse_color
        f.write("// " + mat.name + "\nglColor3f( "+", ".join("{:.15f}".format(c) for c in mat_color) + " );\n")
        
        # For all faces with a certain material, add all vertices, and the normal vector
        for face_idx in matDict[mat.name]:
            face = mesh.polygons[face_idx]
            str_to_write = "\tglNormal3f( " + ", ".join("{0:.15f}".format(vect) for vect in face.normal) + " );\n"
            for vert in face.vertices:
                str_to_write += "\t\tglVertex3f( " + ", ".join("{0:.15f}".format(coord) for coord in vertDict[vert]) + ");\n"
            str_to_write += "\n"
            f.write(str_to_write)
    f.close()

    return {'FINISHED'}

# Default exporter code
# ExportHelper is a helper class, defines filename and
# invoke() function which calls the file selector.
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty, BoolProperty, EnumProperty
from bpy.types import Operator


class ExportSomeData(Operator, ExportHelper):
    """This appears in the tooltip of the operator and in the generated docs"""
    bl_idname = "export_test.some_data"  # important since its how bpy.ops.import_test.some_data is constructed
    bl_label = "Export OpenGL"

    # ExportHelper mixin class uses this
    filename_ext = ".cpp"

    filter_glob = StringProperty(
            default="*.cpp",
            options={'HIDDEN'},
            )

    # List of operator properties, the attributes will be assigned
    # to the class instance from the operator settings before calling.
    use_setting = BoolProperty(
            name="Example Boolean",
            description="Example Tooltip",
            default=True,
            )

    type = EnumProperty(
            name="Example Enum",
            description="Choose between two items",
            items=(('OPT_A', "First Option", "Description one"),
                   ('OPT_B', "Second Option", "Description two")),
            default='OPT_A',
            )

    def execute(self, context):
        return openGL_exporter(context, self.filepath, self.use_setting)


# Only needed if you want to add into a dynamic menu
def menu_func_export(self, context):
    self.layout.operator(ExportSomeData.bl_idname, text="Text Export Operator")


def register():
    bpy.utils.register_class(ExportSomeData)
    bpy.types.INFO_MT_file_export.append(menu_func_export)


def unregister():
    bpy.utils.unregister_class(ExportSomeData)
    bpy.types.INFO_MT_file_export.remove(menu_func_export)


if __name__ == "__main__":
    register()

    # test call
    bpy.ops.export_test.some_data('INVOKE_DEFAULT')

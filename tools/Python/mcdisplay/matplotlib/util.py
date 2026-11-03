''' Helper module to support McStas trace output processing '''

from sys import stdin, stderr
from math import pi, cos, sin
from numpy import dot, array
import numpy as np

#level of detail in linspace
NUM_SAMPLES = 100


def parse_multiline(line):
    ''' Parse a multiline with size as first elements and n points as rest '''
    elems = [float(x) for x in line.split(',')]
    count = int(elems.pop(0))
    points = []
    while count > 0:
        points.append(elems[0:3])
        elems = elems[3:]
        count -= 1

    points.append(points[0])
    return points


def rotate(point, inps):
    ''' Rotate and move v according to origin and rotation matrix '''
    (origin, rotm)=inps
    return dot(point, rotm) + origin


def rotate_points(points, inps):
    ''' Rotate and move v according to origin and rotation matrix '''
    (origin, rotm)=inps
    count = 0
    rpoints=[]
    x=[]
    y=[]
    z=[]
    while count < len(points):
        p=points[count]
        rpoints.append(rotate(p, (origin, rotm)))
        p=rpoints[count]
        x.append(p[0])
        y.append(p[1])
        z.append(p[2])
        count +=1
    x.append(x[0])
    y.append(y[0])
    z.append(z[0])
    return x, y, z

'''BEGIN NEW CODE 3D-visualization. REMOVE OLD CODE AND THIS COMMENT AFTER CONVERTING COMPS'''


def rotate_xyz(x, y, z, comp):
    if x.ndim == 2:  # Handle 2D arrays
        for i in range(len(x)):
            for j in range(len(x[0])):
                point = np.array([x[i][j], y[i][j], z[i][j]])
                rotated_point = rotate(point, comp)
                x[i][j], y[i][j], z[i][j] = rotated_point
    else:  # Handle 1D arrays
        for i in range(len(x)):
            point = np.array([x[i], y[i], z[i]])
            rotated_point = rotate(point, comp)
            x[i], y[i], z[i] = rotated_point
    return x, y, z


def draw_sphere(center, radius):
    u = np.linspace(0, 2 * np.pi, NUM_SAMPLES)
    v = np.linspace(0, np.pi, NUM_SAMPLES)
    x = center[0] + radius * np.outer(np.cos(u), np.sin(v))
    y = center[1] + radius * np.outer(np.sin(u), np.sin(v))
    z = center[2] + radius * np.outer(np.ones(np.size(u)), np.cos(v))
    return x, y, z


def draw_cylinder(center, radius, height, axis_vector):
    axis_vector_normalized = axis_vector / np.linalg.norm(axis_vector)

    # Calculate half of the height vector
    half_height_vector = (height / 2) * axis_vector_normalized

    # Calculate the startpoint
    p0 = center - half_height_vector

    t = np.linspace(0, height, NUM_SAMPLES)
    theta = np.linspace(0, 2 * np.pi, NUM_SAMPLES)
    t, theta = np.meshgrid(t, theta)

    n1, n2 = calc_perp_vectors(axis_vector)

    x = p0[0] + axis_vector_normalized[0] * t + radius * np.sin(theta) * n1[0] + radius * np.cos(theta) * n2[0]
    y = p0[1] + axis_vector_normalized[1] * t + radius * np.sin(theta) * n1[1] + radius * np.cos(theta) * n2[1]
    z = p0[2] + axis_vector_normalized[2] * t + radius * np.sin(theta) * n1[2] + radius * np.cos(theta) * n2[2]

    return x, y, z


def draw_annulus(center, outer_radius, inner_radius, axis_vector):
    # Polar coordinates in the annulus' plane
    theta = np.linspace(0, 2 * np.pi, NUM_SAMPLES)
    r = np.linspace(0, inner_radius, NUM_SAMPLES)
    theta, r = np.meshgrid(theta, r)

    # Calculate coordinates in the annulus's plane
    x_plane = (outer_radius-r) * np.cos(theta)
    y_plane = (outer_radius-r) * np.sin(theta)

    (x, y, z) = center_and_align_with_axis_vector(center, x_plane, y_plane, axis_vector)

    return x, y, z


def draw_disc(center, radius, axis_vector):
    return draw_annulus(center, radius, radius, axis_vector)


def draw_new_circle(center, radius, axis_vector):
    return draw_annulus(center, radius, 0.01, axis_vector)


def draw_cone(center, radius, height, axis_vector):
    # Define the grid in polar coordinates
    theta = np.linspace(0, 2 * np.pi, NUM_SAMPLES)
    z = np.linspace(-height / 2, height / 2, NUM_SAMPLES)
    theta, z = np.meshgrid(theta, z)

    # Convert polar to Cartesian coordinates (original, along z-axis)
    x_plane = (radius * (z - height/2) / height) * np.cos(theta)
    y_plane = (radius * (z - height/2) / height) * np.sin(theta)

    # Compute the two perpendicular vectors
    n1, n2 = calc_perp_vectors(axis_vector)

    # Transform these coordinates to align with the given axis_vector
    x = center[0] + x_plane * n1[0] + y_plane * n2[0] + z * axis_vector[0]
    y = center[1] + x_plane * n1[1] + y_plane * n2[1] + z * axis_vector[1]
    z = center[2] + x_plane * n1[2] + y_plane * n2[2] + z * axis_vector[2]

    return x, y, z


def draw_box(center, a, b, c, axis_vector):
    ''' closed box (faces, vertices) of size a x b x c, its local y axis rotated onto axis_vector
        (smallest rotation, half turn around z for -y), as in the webgl viewers and --trace=1 '''
    n = np.asarray(axis_vector, dtype=float)
    n = n / np.linalg.norm(n)
    if n[1] < -1 + 1e-12:
        rot = np.diag([-1.0, -1.0, 1.0])
    else:
        k = np.array([[0, n[0], 0], [-n[0], 0, -n[2]], [0, n[2], 0]])  # cross((0,1,0), n) as a matrix
        rot = np.eye(3) + k + k @ k / (1 + n[1])
    vertices = np.array([[sx*a/2, sy*b/2, sz*c/2] for sx in (-1, 1) for sy in (-1, 1) for sz in (-1, 1)])
    vertices = vertices @ rot.T + center
    faces = [[0, 1, 3, 2], [4, 5, 7, 6], [0, 1, 5, 4], [2, 3, 7, 6], [0, 2, 6, 4], [1, 3, 7, 5]]
    return faces, vertices


def center_and_align_with_axis_vector(center, x_plane, y_plane, axis_vector):
    # Calculate perpendicular vectors
    n1, n2 = calc_perp_vectors(axis_vector)
    # Transform the coordinates
    x = center[0] + x_plane * n1[0] + y_plane * n2[0]
    y = center[1] + x_plane * n1[1] + y_plane * n2[1]
    z = center[2] + x_plane * n1[2] + y_plane * n2[2]
    return x, y, z

def calc_perp_vectors(axis_vector):
    # Normalize the axis vector
    axis_vector_normalized = axis_vector / np.linalg.norm(axis_vector)
    # Create an arbitrary vector perpendicular to the axis vector
    if (axis_vector_normalized == np.array([1, 0, 0])).all():
        not_v = np.array([0, 1, 0])
    else:
        not_v = np.array([1, 0, 0])
    # Compute two orthogonal vectors in the plane perpendicular to the axis vector
    n1 = np.cross(axis_vector_normalized, not_v)
    n1 /= np.linalg.norm(n1)  # Normalize n1
    n2 = np.cross(axis_vector_normalized, n1)
    return n1, n2

'''END NEW CODE 3D-visualization. REMOVE OLD CODE AND THIS COMMENT AFTER CONVERTING COMPS'''

POINTS_IN_CIRCLE = 128
def draw_circle(plane, pos, radius, comp):
    ''' Draw a circle in plane, at pos and with r radius, rotated by comp '''
    first = None
    x=[]
    y=[]
    z=[]
    for i in range(0, POINTS_IN_CIRCLE):
        walk = 2 * pi * i / POINTS_IN_CIRCLE
        xyz = array(pos)
        xyz[plane[0]] += cos(walk) * radius
        xyz[plane[1]] += sin(walk) * radius
        # rotate
        xyz = rotate(xyz, comp)
        x.append(xyz[0])
        y.append(xyz[1])
        z.append(xyz[2])
    x.append(x[0]);
    y.append(y[0]);
    z.append(z[0]);
    return x,y,z


def get_line():
    ''' Read a line from stdin, None at end of input '''
    line = stdin.readline()
    return line.strip() if line else None


def debug(obj):
    ''' Write a Python object to stderr '''
    stderr.write(repr(obj) + '\n')

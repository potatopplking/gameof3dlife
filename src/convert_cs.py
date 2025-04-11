import numpy as np

def cartesian_to_spherical(x, y, z):
    """
    Convert 3D Cartesian coordinates to spherical coordinates.

    Returns:
        r: radial distance
        theta: polar angle (inclination) in radians
        phi: azimuthal angle in radians
    """
    r = np.sqrt(x**2 + y**2 + z**2)
    theta = np.arccos(z / r)  # inclination angle (from z-axis)
    phi = np.arctan2(y, x)    # azimuthal angle

    return r, theta, phi

def spherical_to_cartesian(r, theta, phi):
    """
    Convert spherical coordinates to 3D Cartesian coordinates.

    Args:
        r: radial distance
        theta: polar angle (inclination) in radians
        phi: azimuthal angle in radians

    Returns:
        x, y, z: Cartesian coordinates
    """
    x = r * np.sin(theta) * np.cos(phi)
    y = r * np.sin(theta) * np.sin(phi)
    z = r * np.cos(theta)

    return x, y, z

# Example usage
#x, y, z = 12.0, 34.0, 56.0
#r, theta, phi = cartesian_to_spherical(x, y, z)
#print(f"Cartesian: ({x}, {y}, {z})")
#print(f"Spherical: r={r}, theta={theta} rad, phi={phi} rad")
#print(f"Spherical: r={r}, theta={np.degrees(theta)}°, phi={np.degrees(phi)}°")

from sys import argv

r, theta, phi = (float(x) for x in argv[1:])
x, y, z = spherical_to_cartesian(r, theta, phi)

print(f"Spherical to cartesian:\n{{ {r} {theta} {phi} }} -> {{ {x} {y} {z} }}")

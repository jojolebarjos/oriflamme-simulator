from glob import glob
from setuptools import find_packages, setup, Extension

extensions = [
    Extension(
        name="oriflamme",
        sources=[
            *sorted(glob("src/*.c")),
        ],
    ),
]


setup(
    name="oriflamme",
    version="0.1.0",
    packages=find_packages(),
    python_requires=">=3.6",
    ext_modules=extensions,
)

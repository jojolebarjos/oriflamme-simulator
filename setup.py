
from setuptools import find_packages, setup, Extension
from glob import glob


extensions = [
    Extension(
        name="oriflamme.core",
        sources=[
            *glob("oriflamme/core/*.c"),
        ],
    ),
]

setup(
    name="oriflamme",
    version="0.0.1",
    packages=find_packages(),
    author="Johan Berdat",
    author_email="jojolebarjos@gmail.com",
    license="MIT",
    python_requires=">=3.7",
    install_requires=[],
    ext_modules=extensions,
)

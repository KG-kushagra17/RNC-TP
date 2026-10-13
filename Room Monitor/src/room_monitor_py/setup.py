from setuptools import find_packages, setup

package_name = 'room_monitor_py'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='kushagra',
    maintainer_email='kushagra@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': ['temperature_sensor = room_monitor_py.temperature_sensor:main',
        'humidity_sensor = room_monitor_py.humidity_sensor:main',
        'smoke_sensor = room_monitor_py.smoke_sensor:main',
        'room_dashboard = room_monitor_py.dashboard:main',
        'status_client = room_monitor_py.status_client:main',
        ],
    },
)

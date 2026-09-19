from setuptools import find_packages, setup

package_name = 'smart_home'

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
    'console_scripts': [
        'temperature_sensor = smart_home.temperature_sensor:main',
        'smart_plug_controller = smart_home.smart_plug_controller:main',
	'home_dashboard= smart_home.home_dashboard:main',
    ],
},)


from setuptools import find_packages, setup, find_packages
import os

package_name = 'audio_playback_service'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        #wav folder 추가
        (os.path.join('share', package_name, 'wav'), ['wav/1_track.wav', 'wav/2_track.wav', 'wav/3_track.wav']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='nvidia',
    maintainer_email='nvidia@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
        'console_scripts': [
            'audio_service = audio_playback_service.audio_service:main',
        ],
    },
)

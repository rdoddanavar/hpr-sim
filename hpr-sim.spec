# -*- mode: python ; coding: utf-8 -*-


a = Analysis(
    ['hpr-sim.py'],
    pathex=['src/exec','src/preproc','src/postproc','src/util'],
    binaries=[('build/src/model.cpython-310-x86_64-linux-gnu.so','.')],
    datas=[('build/CMakeCache.txt','.'),
           ('config/config_input.yml','config'),
           ('config/config_output.yml','config'),
           ('config/config_unit.yml','config')],
    hiddenimports=[],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
    optimize=0,
)
pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='hpr-sim',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=True,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)
coll = COLLECT(
    exe,
    a.binaries,
    a.datas,
    strip=False,
    upx=True,
    upx_exclude=[],
    name='hpr-sim',
)

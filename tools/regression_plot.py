import sys
import pathlib

postprocPath = pathlib.Path(__file__).parent.parent / "src" / "postproc"

sys.path.append(postprocPath.resolve().as_posix())

import postproc_flight

postproc_flight.plot_pdf(pathlib.Path("output/unit_test"))

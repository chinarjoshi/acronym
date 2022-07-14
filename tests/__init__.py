import sys
from pathlib import Path

path = Path(__file__).parent.parent.resolve() / 'acronym'
sys.path.append(str(path))

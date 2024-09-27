make clean
make test_mul
python3 parser.py
make cpp
./rv32imb --debug dec -rd

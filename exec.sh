if [ -z "$1" ]; then
    echo "Erreur : Argument manquant. Par exemple : ./exec.sh q1. Option possible : q1, q2, q3 et q4"
    echo "Peut prendre deux arguements pour q3 : ./exec.sh q3 m100 ou ./exec.sh q3 g1"
    exit 1
fi

if [ -z "$2" ]; then
    gcc -O0 -D $1 ssdLimitsTest.c -o ssdLimitsTest
else 
    gcc -O0 -D $1 -D $2 ssdLimitsTest.c -o ssdLimitsTest
fi
./ssdLimitsTest
cat res.txt

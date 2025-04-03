if [ -z "$1" ]; then
    echo "Erreur : Argument manquant. Par exemple : ./exec.sh 1. Option possible : 1, 2 et 3"
    exit 1
fi

gcc -O0 -D $1 ssdLimitsTest.c -o ssdLimitsTest
./ssdLimitsTest
cat res.txt

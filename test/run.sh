set -e

for file in $(ls test/*.c); do
  echo "Running test: ${file}"
  cc -std=c99 -Wall ${file} src/*.c -I include -ledit -lm -o ${file}.test;
  echo ""

  ${file}.test;
  echo ""
done

echo ""
echo "All tests PASSED"

benches = [
  'insert',
]

foreach b : benches
  b_exe = executable(
    b,
    '@0@.cpp'.format(b),
    dependencies: [matter_dep, google_benchmark_dep],
  )
  test(b, b_exe)
endforeach

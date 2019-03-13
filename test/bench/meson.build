benches = [
  'insert',
]

foreach b : benches
  b_exe = executable(
    b,
    '@0@.cpp'.format(b),
    dependencies: [matter, google_benchmark_dep],
  )
endforeach

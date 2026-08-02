# Canonicalize semantically equivalent Pandoc man-writer output.
#
# Pandoc 3.x releases differ in two presentation details used by this project:
# syntax-highlighting escapes may appear inside .EX/.EE blocks, and bullet lists
# may use either \[bu] or \(bu. Neither difference is part of the manual-page
# contract, so normalize both before comparing or committing generated roff.

/^\.EX$/ {
  in_example = 1
  print
  next
}

/^\.EE$/ {
  in_example = 0
  print
  next
}

{
  line = $0

  if (in_example) {
    gsub(/\\f\[[^]]+\]/, "", line)
  }

  if (line == ".IP \\(bu 2") {
    line = ".IP \\[bu] 2"
  }

  print line
}

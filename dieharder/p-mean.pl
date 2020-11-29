#!/usr/bin/perl
#for $f (<*_a_Y1.out.txt>,<*_a.out.txt>)
use strict;
my (@F, @S, @A, $prev_rng);

sub mean {
  my $cnt = scalar @_;
  my $sum = 0.0;
  $sum += $_ for @_;
  #warn "mean ($cnt): ", $sum / $cnt, "\n";
  return $sum / $cnt;
}

sub stddev {
  my $cnt = scalar @_;
  my $sum = 0.0;
  $sum += ($_ * $_) for @_;
  return sqrt($sum / $cnt);
}

while (<>) {
  @F = split(/\|/);
  next if @F != 6;
  next if $F[5] !~ /(PASSED|WEAK|FAIL)/;
  next if $F[4] !~ /^0\./;
  my $s = 0.5 - abs($F[4] - 0.5);
  if ($prev_rng eq $F[0]) {
    #warn "$prev_rng $s\n";
    push @A, $s;
  } else {
    if (@A) {
      $s = mean (pop @S, @A);
      @A = ();
    }
    #warn "$F[0] $s\n";
    push @S, $s;
  }
  $prev_rng = $F[0];
}

printf "%.6f  |%.6f  |\n", mean (@S), stddev (@S); 

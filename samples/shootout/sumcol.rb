#!/usr/bin/ruby
# -*- mode: ruby -*-
# $Id: sumcol-ruby.code,v 1.12 2006/09/20 05:52:42 bfulgham Exp $
# http://www.bagley.org/~doug/shootout/
# from: Mathieu Bouchard, revised by Dave Anderson

count = 0
l=""
STDIN.each{ |l|
    count += l.to_i
}
puts count
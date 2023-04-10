#!/bin/bash

# Copyright Hash-LSH Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.


###########################################################################
#
# Author: Adan Hirales Carbajal
# Email : adan.hirales@cetys.mx
#
###########################################################################

# This script is very simple. I user awk to assign a unique integer (Z) label
# to a unique string stores stored in lexicographical order. The script syntax
# is the following:
#
# ./Z_inverted_index.sh source_dictionary inverted_index_file
#
#

# Evaluate if arguments are given
[ $# < 3 ] && {echo "Usage: $0 source_dictionary target_dictionary"; exit 1; }

# Assign integer labels to the strings in the dictionary
cat $1 | awk 'BEGIN { count=0 } { count++; print $0 "," count }' > $2

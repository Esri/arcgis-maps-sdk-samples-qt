# Copyright 2020 Esri.

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

all  # opt-in all rules by default
rule 'MD003', :style => :atx  # Header style written as non-closing pound marks. e.g. ## Section title
rule 'MD004', :style => :asterisk  # Unordered list style as asterisk, rather than hyphen or plus sign
rule 'MD029', :style => :ordered  # Ordered list item prefix is incremental, rather than all ones
exclude_rule 'MD013'  # not limiting line length
exclude_rule 'MD007'  # not limiting unordered list indentation, tab, 2 or 4 spaces are all fine

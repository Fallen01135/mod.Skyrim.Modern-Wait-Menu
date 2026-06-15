import csv
import os

# --------------------------------
# Definition area for 
# --------------------------------

# If the Settings should be defined "compact" in one line, or not compact over multiple lines.
# Settings this option to "False" can increase readibility, but increases number of lines drastically.
COMPACT = False

# If True all settings will be grouped into sub structs named on their sections
# for example: Settings::General::mySetting
GROUP_BY_SECTION = False

# The name of the tsv file to use the settings from
INPUT_FILE = 'settings.tsv'
# Path for the created file
OUTPUT_FILE = '../src/Settings.hpp'
# The path for the include statement
LIBRARY_PATH = '../include/SimpleSettings.hpp'

# The name the namespace of the library has. Normally should not be changed. Only change if you know what you are doing.
LIB_NAMESPACE = 'SettingsLib'
# The name of the namespace the settings struct should be in, can be empty, then no namespace will be generated
NAMESPACE = 'ModernWaitMenu'
# The name of the struct where the settings should be defined.
STRUCTNAME = 'Settings'
# --------------------------------
# --------------------------------
# --------------------------------



# --------------------------------
# Code following now
# --------------------------------
settings = []

if not os.path.exists(INPUT_FILE):
	print(f"Error: {INPUT_FILE} not found!")
	exit(1)

with open(INPUT_FILE, mode='r', encoding='utf-8-sig') as f:
	reader = csv.DictReader(f, delimiter='\t')
	for row in reader:
		clean_row = {k.strip(): v.strip() for k, v in row.items()}
		settings.append(clean_row)

def get_indent(level):
	base = "	" if not NAMESPACE.strip() else "		"
	return base + ("	" * level)

sections = {}
for s in settings:
	sec = s["Section"]
	if sec not in sections:
		sections[sec] = []
	sections[sec].append(s)

items_code = ""
tuple_entries = []

for sec_name, sec_items in sections.items():
	if GROUP_BY_SECTION:
		items_code += f"{get_indent(0)}struct {sec_name}\n{get_indent(0)}{{\n"
    
	for s in sec_items:
		comment_text = s.get("Comment", "")
		comment_val = f'"{comment_text}"' if comment_text else "nullptr"

		condition_text = s.get("Condition", "").strip()
		validator_val = f'[](const {s["Type"]}& value) {{ return {condition_text}; }}' if condition_text else "nullptr"
        
		type_str = s["Type"]
		type_lower = type_str.lower()
		is_vector = "vector" in type_lower
		val = s["Default"]

		is_hex_bool = s.get("IsHex", "false").strip().lower() == "true"
		is_hex_str = "true" if is_hex_bool else "false"

		if "string" in type_lower and not is_vector:
			val = f'"{val}"'
		elif "bool" in type_lower:
			val = val.lower()

		if is_vector:
			if "string" in type_lower:
				parts = [f'"{p.strip()}"' for p in val.split(',')]
				val = ", ".join(parts)
			val = f"{{ {val} }}"

		name = s["Name"]
        
		curr_indent = get_indent(1) if GROUP_BY_SECTION else get_indent(0)
        
		args_list = [f'"{sec_name}"', f'"{name}"', val]
		show_hex = is_hex_bool
		show_validator = show_hex or (validator_val != "nullptr")
		show_comment = show_validator or (comment_text.strip() != "")

		if show_comment:
			args_list.append(comment_val)
		if show_validator:
			args_list.append(validator_val)
		if show_hex:
			args_list.append(is_hex_str)

		if COMPACT:
			args_string = ", ".join(args_list)
			items_code += f'{curr_indent}inline static {LIB_NAMESPACE}::Item<{type_str}> {name}{{ {args_string} }};\n'
		else:
			items_code += f'{curr_indent}inline static {LIB_NAMESPACE}::Item<{type_str}> {name}\n'
			items_code += f'{curr_indent}{{\n'

			for i, arg in enumerate(args_list):
					comma = "," if i < len(args_list) - 1 else ""
					items_code += f'{curr_indent}    {arg}{comma}\n'
			items_code += f'{curr_indent}}};\n\n'

		ref_path = f"{sec_name}::{name}" if GROUP_BY_SECTION else name
		tuple_indent = "			" if NAMESPACE.strip() else "		"
		tuple_entries.append(f'{tuple_indent}std::ref({ref_path})')

		if (GROUP_BY_SECTION):
			items_code += f"{get_indent(0)}}};\n\n"

tuple_code = ",\n".join(tuple_entries)

content_template = f"""#pragma once


#include "{LIBRARY_PATH}"
#include <functional>
#include <string>
#include <tuple>
#include <vector>


/**
* GENERATED CODE - DO NOT MODIFY MANUALLY.
* Source: {INPUT_FILE}
*/
"""

if NAMESPACE.strip():
	content_template += f"namespace {NAMESPACE}\n{{\n"

content_template += f"""	struct {STRUCTNAME}
	{{
{items_code}
		static constexpr auto ALL = std::make_tuple
		(
{tuple_code}
		);
	}};
"""

if NAMESPACE.strip():
	content_template += "}\n"

with open(OUTPUT_FILE, 'w', encoding='utf-8') as f:
	f.write(content_template)

print(f"{OUTPUT_FILE} successfully generated from {INPUT_FILE}!")
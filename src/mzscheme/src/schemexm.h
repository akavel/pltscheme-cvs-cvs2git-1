#define scheme_init_jmpup_buf (scheme_extension_table->scheme_init_jmpup_buf)
#define scheme_setjmpup_relative (scheme_extension_table->scheme_setjmpup_relative)
#define scheme_longjmpup (scheme_extension_table->scheme_longjmpup)
#define scheme_reset_jmpup_buf (scheme_extension_table->scheme_reset_jmpup_buf)
#ifdef USE_MZ_SETJMP
#define scheme_setjmp (scheme_extension_table->scheme_setjmp)
#define scheme_longjmp (scheme_extension_table->scheme_longjmp)
#endif
#define scheme_clear_escape (scheme_extension_table->scheme_clear_escape)
#define scheme_make_config (scheme_extension_table->scheme_make_config)
#define scheme_branch_config (scheme_extension_table->scheme_branch_config)
#define scheme_new_param (scheme_extension_table->scheme_new_param)
#define scheme_param_config (scheme_extension_table->scheme_param_config)
#define scheme_register_parameter (scheme_extension_table->scheme_register_parameter)
#define scheme_get_env (scheme_extension_table->scheme_get_env)
#ifdef MZ_REAL_THREADS
#define scheme_get_current_process (scheme_extension_table->scheme_get_current_process)
#else
#ifndef LINK_EXTENSIONS_BY_TABLE
#define scheme_current_process (scheme_extension_table->scheme_current_process)
#define scheme_fuel_counter (scheme_extension_table->scheme_fuel_counter)
#else
#define scheme_current_process_ptr (scheme_extension_table->scheme_current_process_ptr)
#define scheme_fuel_counter_ptr (scheme_extension_table->scheme_fuel_counter_ptr)
#endif
#endif
#define scheme_make_namespace (scheme_extension_table->scheme_make_namespace)
#ifndef NO_SCHEME_THREADS
#define scheme_thread (scheme_extension_table->scheme_thread)
#define scheme_thread_w_manager (scheme_extension_table->scheme_thread_w_manager)
#define scheme_kill_thread (scheme_extension_table->scheme_kill_thread)
#endif
#define scheme_break_thread (scheme_extension_table->scheme_break_thread)
#ifndef MZ_REAL_THREADS
#define scheme_process_block (scheme_extension_table->scheme_process_block)
#define scheme_swap_process (scheme_extension_table->scheme_swap_process)
#else
#define scheme_process_block_w_process (scheme_extension_table->scheme_process_block_w_process)
#endif
#define scheme_weak_suspend_thread (scheme_extension_table->scheme_weak_suspend_thread)
#define scheme_weak_resume_thread (scheme_extension_table->scheme_weak_resume_thread)
#define scheme_block_until (scheme_extension_table->scheme_block_until)
#define scheme_in_main_thread (scheme_extension_table->scheme_in_main_thread)
#define scheme_tls_allocate (scheme_extension_table->scheme_tls_allocate)
#define scheme_tls_set (scheme_extension_table->scheme_tls_set)
#define scheme_tls_get (scheme_extension_table->scheme_tls_get)
#define scheme_add_namespace_option (scheme_extension_table->scheme_add_namespace_option)
#define scheme_make_manager (scheme_extension_table->scheme_make_manager)
#define scheme_add_managed (scheme_extension_table->scheme_add_managed)
#define scheme_remove_managed (scheme_extension_table->scheme_remove_managed)
#define scheme_close_managed (scheme_extension_table->scheme_close_managed)
#define scheme_signal_error (scheme_extension_table->scheme_signal_error)
#define scheme_raise_exn (scheme_extension_table->scheme_raise_exn)
#define scheme_warning (scheme_extension_table->scheme_warning)
#define scheme_wrong_count (scheme_extension_table->scheme_wrong_count)
#define scheme_case_lambda_wrong_count (scheme_extension_table->scheme_case_lambda_wrong_count)
#define scheme_wrong_type (scheme_extension_table->scheme_wrong_type)
#define scheme_arg_mismatch (scheme_extension_table->scheme_arg_mismatch)
#define scheme_wrong_return_arity (scheme_extension_table->scheme_wrong_return_arity)
#define scheme_unbound_global (scheme_extension_table->scheme_unbound_global)
#define scheme_dynamic_wind (scheme_extension_table->scheme_dynamic_wind)
#define scheme_make_type (scheme_extension_table->scheme_make_type)
#define scheme_install_type_reader (scheme_extension_table->scheme_install_type_reader)
#define scheme_install_type_writer (scheme_extension_table->scheme_install_type_writer)
#define scheme_eof (scheme_extension_table->scheme_eof)
#define scheme_null (scheme_extension_table->scheme_null)
#define scheme_true (scheme_extension_table->scheme_true)
#define scheme_false (scheme_extension_table->scheme_false)
#define scheme_void (scheme_extension_table->scheme_void)
#define scheme_undefined (scheme_extension_table->scheme_undefined)
#define scheme_tail_call_waiting (scheme_extension_table->scheme_tail_call_waiting)
#define scheme_multiple_values (scheme_extension_table->scheme_multiple_values)
#define scheme_eval (scheme_extension_table->scheme_eval)
#define scheme_eval_multi (scheme_extension_table->scheme_eval_multi)
#define scheme_eval_compiled (scheme_extension_table->scheme_eval_compiled)
#define scheme_eval_compiled_multi (scheme_extension_table->scheme_eval_compiled_multi)
#define _scheme_eval_compiled (scheme_extension_table->_scheme_eval_compiled)
#define _scheme_eval_compiled_multi (scheme_extension_table->_scheme_eval_compiled_multi)
#ifndef MZ_REAL_THREADS
#define scheme_apply (scheme_extension_table->scheme_apply)
#define scheme_apply_multi (scheme_extension_table->scheme_apply_multi)
#define scheme_apply_eb (scheme_extension_table->scheme_apply_eb)
#define scheme_apply_multi_eb (scheme_extension_table->scheme_apply_multi_eb)
#else
#define scheme_apply_wp (scheme_extension_table->scheme_apply_wp)
#define scheme_apply_multi_wp (scheme_extension_table->scheme_apply_multi_wp)
#define scheme_apply_eb_wp (scheme_extension_table->scheme_apply_eb_wp)
#define scheme_apply_multi_eb_wp (scheme_extension_table->scheme_apply_multi_eb_wp)
#endif
#define scheme_apply_to_list (scheme_extension_table->scheme_apply_to_list)
#define scheme_eval_string (scheme_extension_table->scheme_eval_string)
#define scheme_eval_string_all (scheme_extension_table->scheme_eval_string_all)
#define _scheme_apply_known_closed_prim (scheme_extension_table->_scheme_apply_known_closed_prim)
#define _scheme_apply_known_closed_prim_multi (scheme_extension_table->_scheme_apply_known_closed_prim_multi)
#define _scheme_apply_closed_prim (scheme_extension_table->_scheme_apply_closed_prim)
#define _scheme_apply_closed_prim_multi (scheme_extension_table->_scheme_apply_closed_prim_multi)
#define scheme_values (scheme_extension_table->scheme_values)
#define scheme_check_one_value (scheme_extension_table->scheme_check_one_value)
#define scheme_tail_apply (scheme_extension_table->scheme_tail_apply)
#define scheme_tail_apply_no_copy (scheme_extension_table->scheme_tail_apply_no_copy)
#define scheme_tail_apply_to_list (scheme_extension_table->scheme_tail_apply_to_list)
#define scheme_tail_eval_expr (scheme_extension_table->scheme_tail_eval_expr)
#define scheme_set_tail_buffer_size (scheme_extension_table->scheme_set_tail_buffer_size)
#define scheme_force_value (scheme_extension_table->scheme_force_value)
#ifndef MZ_REAL_THREADS
#define scheme_do_eval (scheme_extension_table->scheme_do_eval)
#else
#define scheme_do_eval_w_process (scheme_extension_table->scheme_do_eval_w_process)
#endif
#ifndef SCHEME_NO_GC
#ifndef SCHEME_NO_GC_PROTO
#define GC_malloc (scheme_extension_table->GC_malloc)
#define GC_malloc_atomic (scheme_extension_table->GC_malloc_atomic)
#define GC_malloc_stubborn (scheme_extension_table->GC_malloc_stubborn)
#define GC_malloc_uncollectable (scheme_extension_table->GC_malloc_uncollectable)
#endif
#endif
#define scheme_malloc_eternal (scheme_extension_table->scheme_malloc_eternal)
#define scheme_end_stubborn_change (scheme_extension_table->scheme_end_stubborn_change)
#define scheme_calloc (scheme_extension_table->scheme_calloc)
#define scheme_strdup (scheme_extension_table->scheme_strdup)
#define scheme_strdup_eternal (scheme_extension_table->scheme_strdup_eternal)
#define scheme_malloc_fail_ok (scheme_extension_table->scheme_malloc_fail_ok)
#define scheme_weak_reference (scheme_extension_table->scheme_weak_reference)
#define scheme_weak_reference_indirect (scheme_extension_table->scheme_weak_reference_indirect)
#define scheme_add_finalizer (scheme_extension_table->scheme_add_finalizer)
#define scheme_add_finalizer_once (scheme_extension_table->scheme_add_finalizer_once)
#define scheme_add_scheme_finalizer (scheme_extension_table->scheme_add_scheme_finalizer)
#define scheme_add_scheme_finalizer_once (scheme_extension_table->scheme_add_scheme_finalizer_once)
#define scheme_register_finalizer (scheme_extension_table->scheme_register_finalizer)
#define scheme_dont_gc_ptr (scheme_extension_table->scheme_dont_gc_ptr)
#define scheme_gc_ptr_ok (scheme_extension_table->scheme_gc_ptr_ok)
#define scheme_collect_garbage (scheme_extension_table->scheme_collect_garbage)
#define scheme_hash_table (scheme_extension_table->scheme_hash_table)
#define scheme_add_to_table (scheme_extension_table->scheme_add_to_table)
#define scheme_change_in_table (scheme_extension_table->scheme_change_in_table)
#define scheme_lookup_in_table (scheme_extension_table->scheme_lookup_in_table)
#define scheme_bucket_from_table (scheme_extension_table->scheme_bucket_from_table)
#define scheme_make_prim (scheme_extension_table->scheme_make_prim)
#define scheme_make_noneternal_prim (scheme_extension_table->scheme_make_noneternal_prim)
#define scheme_make_closed_prim (scheme_extension_table->scheme_make_closed_prim)
#define scheme_make_prim_w_arity (scheme_extension_table->scheme_make_prim_w_arity)
#define scheme_make_folding_prim (scheme_extension_table->scheme_make_folding_prim)
#define scheme_make_noneternal_prim_w_arity (scheme_extension_table->scheme_make_noneternal_prim_w_arity)
#define scheme_make_closed_prim_w_arity (scheme_extension_table->scheme_make_closed_prim_w_arity)
#define scheme_make_folding_closed_prim (scheme_extension_table->scheme_make_folding_closed_prim)
#define scheme_make_closure (scheme_extension_table->scheme_make_closure)
#define scheme_make_pair (scheme_extension_table->scheme_make_pair)
#define scheme_make_string (scheme_extension_table->scheme_make_string)
#define scheme_make_sized_string (scheme_extension_table->scheme_make_sized_string)
#define scheme_make_string_without_copying (scheme_extension_table->scheme_make_string_without_copying)
#define scheme_alloc_string (scheme_extension_table->scheme_alloc_string)
#define scheme_append_string (scheme_extension_table->scheme_append_string)
#define scheme_make_vector (scheme_extension_table->scheme_make_vector)
#define scheme_make_integer_value (scheme_extension_table->scheme_make_integer_value)
#define scheme_make_integer_value_from_unsigned (scheme_extension_table->scheme_make_integer_value_from_unsigned)
#define scheme_make_double (scheme_extension_table->scheme_make_double)
#ifdef MZ_USE_SINGLE_FLOATS
#define scheme_make_float (scheme_extension_table->scheme_make_float)
#endif
#define scheme_make_char (scheme_extension_table->scheme_make_char)
#define scheme_make_promise (scheme_extension_table->scheme_make_promise)
#define scheme_make_promise_from_thunk (scheme_extension_table->scheme_make_promise_from_thunk)
#ifndef NO_SCHEME_THREADS
#define scheme_make_sema (scheme_extension_table->scheme_make_sema)
#define scheme_post_sema (scheme_extension_table->scheme_post_sema)
#define scheme_wait_sema (scheme_extension_table->scheme_wait_sema)
#endif
#define scheme_char_constants (scheme_extension_table->scheme_char_constants)
#define scheme_get_int_val (scheme_extension_table->scheme_get_int_val)
#define scheme_get_unsigned_int_val (scheme_extension_table->scheme_get_unsigned_int_val)
#define scheme_real_to_double (scheme_extension_table->scheme_real_to_double)
#define scheme_get_proc_name (scheme_extension_table->scheme_get_proc_name)
#define scheme_make_bignum (scheme_extension_table->scheme_make_bignum)
#define scheme_make_bignum_from_unsigned (scheme_extension_table->scheme_make_bignum_from_unsigned)
#define scheme_bignum_to_double (scheme_extension_table->scheme_bignum_to_double)
#define scheme_bignum_from_double (scheme_extension_table->scheme_bignum_from_double)
#ifdef MZ_USE_SINGLE_FLOATS
#define scheme_bignum_to_float (scheme_extension_table->scheme_bignum_to_float)
#define scheme_bignum_from_float (scheme_extension_table->scheme_bignum_from_float)
#else
# define scheme_bignum_to_float scheme_bignum_to_double
# define scheme_bignum_from_float scheme_bignum_from_double
#endif
#define scheme_bignum_to_string (scheme_extension_table->scheme_bignum_to_string)
#define scheme_read_bignum (scheme_extension_table->scheme_read_bignum)
#define scheme_bignum_normalize (scheme_extension_table->scheme_bignum_normalize)
#define scheme_double_to_int (scheme_extension_table->scheme_double_to_int)
#define scheme_make_rational (scheme_extension_table->scheme_make_rational)
#define scheme_rational_to_double (scheme_extension_table->scheme_rational_to_double)
#define scheme_rational_from_double (scheme_extension_table->scheme_rational_from_double)
#ifdef MZ_USE_SINGLE_FLOATS
#define scheme_rational_to_float (scheme_extension_table->scheme_rational_to_float)
#define scheme_rational_from_float (scheme_extension_table->scheme_rational_from_float)
#else
# define scheme_rational_to_float scheme_rational_to_double
# define scheme_rational_from_float scheme_rational_from_double
#endif
#define scheme_rational_normalize (scheme_extension_table->scheme_rational_normalize)
#define scheme_rational_numerator (scheme_extension_table->scheme_rational_numerator)
#define scheme_rational_denominator (scheme_extension_table->scheme_rational_denominator)
#define scheme_make_complex (scheme_extension_table->scheme_make_complex)
#define scheme_complex_normalize (scheme_extension_table->scheme_complex_normalize)
#define scheme_complex_real_part (scheme_extension_table->scheme_complex_real_part)
#define scheme_complex_imaginary_part (scheme_extension_table->scheme_complex_imaginary_part)
#define scheme_is_exact (scheme_extension_table->scheme_is_exact)
#define scheme_is_inexact (scheme_extension_table->scheme_is_inexact)
#define scheme_expand (scheme_extension_table->scheme_expand)
#define scheme_compile (scheme_extension_table->scheme_compile)
#define scheme_make_promise_value (scheme_extension_table->scheme_make_promise_value)
#define scheme_read (scheme_extension_table->scheme_read)
#define scheme_write (scheme_extension_table->scheme_write)
#define scheme_display (scheme_extension_table->scheme_display)
#define scheme_write_w_max (scheme_extension_table->scheme_write_w_max)
#define scheme_display_w_max (scheme_extension_table->scheme_display_w_max)
#define scheme_write_string (scheme_extension_table->scheme_write_string)
#define scheme_write_to_string (scheme_extension_table->scheme_write_to_string)
#define scheme_display_to_string (scheme_extension_table->scheme_display_to_string)
#define scheme_write_to_string_w_max (scheme_extension_table->scheme_write_to_string_w_max)
#define scheme_display_to_string_w_max (scheme_extension_table->scheme_display_to_string_w_max)
#define scheme_debug_print (scheme_extension_table->scheme_debug_print)
#define scheme_flush_output (scheme_extension_table->scheme_flush_output)
#define scheme_format (scheme_extension_table->scheme_format)
#define scheme_printf (scheme_extension_table->scheme_printf)
#define scheme_getc (scheme_extension_table->scheme_getc)
#define scheme_ungetc (scheme_extension_table->scheme_ungetc)
#define scheme_char_ready (scheme_extension_table->scheme_char_ready)
#define scheme_need_wakeup (scheme_extension_table->scheme_need_wakeup)
#define scheme_get_chars (scheme_extension_table->scheme_get_chars)
#define scheme_tell (scheme_extension_table->scheme_tell)
#define scheme_output_tell (scheme_extension_table->scheme_output_tell)
#define scheme_tell_line (scheme_extension_table->scheme_tell_line)
#define scheme_close_input_port (scheme_extension_table->scheme_close_input_port)
#define scheme_close_output_port (scheme_extension_table->scheme_close_output_port)
#define scheme_are_all_chars_ready (scheme_extension_table->scheme_are_all_chars_ready)
#define scheme_make_port_type (scheme_extension_table->scheme_make_port_type)
#define scheme_make_input_port (scheme_extension_table->scheme_make_input_port)
#define scheme_make_output_port (scheme_extension_table->scheme_make_output_port)
#define scheme_make_file_input_port (scheme_extension_table->scheme_make_file_input_port)
#define scheme_make_named_file_input_port (scheme_extension_table->scheme_make_named_file_input_port)
#define scheme_make_file_output_port (scheme_extension_table->scheme_make_file_output_port)
#define scheme_make_string_input_port (scheme_extension_table->scheme_make_string_input_port)
#define scheme_make_sized_string_input_port (scheme_extension_table->scheme_make_sized_string_input_port)
#define scheme_make_string_output_port (scheme_extension_table->scheme_make_string_output_port)
#define scheme_get_string_output (scheme_extension_table->scheme_get_string_output)
#define scheme_get_sized_string_output (scheme_extension_table->scheme_get_sized_string_output)
#define scheme_pipe (scheme_extension_table->scheme_pipe)
#define scheme_file_exists (scheme_extension_table->scheme_file_exists)
#define scheme_directory_exists (scheme_extension_table->scheme_directory_exists)
#define scheme_expand_filename (scheme_extension_table->scheme_expand_filename)
#define scheme_os_getcwd (scheme_extension_table->scheme_os_getcwd)
#define scheme_os_setcwd (scheme_extension_table->scheme_os_setcwd)
#define scheme_getdrive (scheme_extension_table->scheme_getdrive)
#define scheme_split_pathname (scheme_extension_table->scheme_split_pathname)
#define scheme_build_pathname (scheme_extension_table->scheme_build_pathname)
#define scheme_alloc_fdset_array (scheme_extension_table->scheme_alloc_fdset_array)
#define scheme_init_fdset_array (scheme_extension_table->scheme_init_fdset_array)
#define scheme_get_fdset (scheme_extension_table->scheme_get_fdset)
#define scheme_fdzero (scheme_extension_table->scheme_fdzero)
#define scheme_fdset (scheme_extension_table->scheme_fdset)
#define scheme_fdclr (scheme_extension_table->scheme_fdclr)
#define scheme_fdisset (scheme_extension_table->scheme_fdisset)
#define scheme_add_global (scheme_extension_table->scheme_add_global)
#define scheme_add_global_constant (scheme_extension_table->scheme_add_global_constant)
#define scheme_add_global_keyword (scheme_extension_table->scheme_add_global_keyword)
#define scheme_remove_global (scheme_extension_table->scheme_remove_global)
#define scheme_remove_global_constant (scheme_extension_table->scheme_remove_global_constant)
#define scheme_add_global_symbol (scheme_extension_table->scheme_add_global_symbol)
#define scheme_remove_global_symbol (scheme_extension_table->scheme_remove_global_symbol)
#define scheme_add_global_constant_symbol (scheme_extension_table->scheme_add_global_constant_symbol)
#define scheme_set_keyword (scheme_extension_table->scheme_set_keyword)
#define scheme_make_envunbox (scheme_extension_table->scheme_make_envunbox)
#define scheme_lookup_global (scheme_extension_table->scheme_lookup_global)
#define scheme_global_bucket (scheme_extension_table->scheme_global_bucket)
#define scheme_set_global_bucket (scheme_extension_table->scheme_set_global_bucket)
#define scheme_intern_symbol (scheme_extension_table->scheme_intern_symbol)
#define scheme_intern_exact_symbol (scheme_extension_table->scheme_intern_exact_symbol)
#define scheme_make_symbol (scheme_extension_table->scheme_make_symbol)
#define scheme_make_exact_symbol (scheme_extension_table->scheme_make_exact_symbol)
#define scheme_symbol_name (scheme_extension_table->scheme_symbol_name)
#define scheme_symbol_name_and_size (scheme_extension_table->scheme_symbol_name_and_size)
#define scheme_make_struct_values (scheme_extension_table->scheme_make_struct_values)
#define scheme_make_struct_names (scheme_extension_table->scheme_make_struct_names)
#define scheme_make_struct_type (scheme_extension_table->scheme_make_struct_type)
#define scheme_make_struct_instance (scheme_extension_table->scheme_make_struct_instance)
#define scheme_is_struct_instance (scheme_extension_table->scheme_is_struct_instance)
#ifndef NO_OBJECT_SYSTEM
#define scheme_is_subclass (scheme_extension_table->scheme_is_subclass)
#define scheme_is_implementation (scheme_extension_table->scheme_is_implementation)
#define scheme_is_interface_extension (scheme_extension_table->scheme_is_interface_extension)
#define scheme_is_a (scheme_extension_table->scheme_is_a)
#define scheme_get_class_name (scheme_extension_table->scheme_get_class_name)
#define scheme_get_interface_name (scheme_extension_table->scheme_get_interface_name)
#define scheme_make_object (scheme_extension_table->scheme_make_object)
#define scheme_make_uninited_object (scheme_extension_table->scheme_make_uninited_object)
#define scheme_find_ivar (scheme_extension_table->scheme_find_ivar)
#define scheme_make_class (scheme_extension_table->scheme_make_class)
#define scheme_add_method (scheme_extension_table->scheme_add_method)
#define scheme_add_method_w_arity (scheme_extension_table->scheme_add_method_w_arity)
#define scheme_made_class (scheme_extension_table->scheme_made_class)
#define scheme_class_to_interface (scheme_extension_table->scheme_class_to_interface)
#define scheme_make_class_assembly (scheme_extension_table->scheme_make_class_assembly)
#define scheme_create_class (scheme_extension_table->scheme_create_class)
#define scheme_make_interface_assembly (scheme_extension_table->scheme_make_interface_assembly)
#define scheme_create_interface (scheme_extension_table->scheme_create_interface)
#define scheme_apply_generic_data (scheme_extension_table->scheme_apply_generic_data)
#define scheme_get_generic_data (scheme_extension_table->scheme_get_generic_data)
#endif
#define scheme_invoke_unit (scheme_extension_table->scheme_invoke_unit)
#define scheme_assemble_compound_unit (scheme_extension_table->scheme_assemble_compound_unit)
#define scheme_make_compound_unit (scheme_extension_table->scheme_make_compound_unit)
#define scheme_get_unit_name (scheme_extension_table->scheme_get_unit_name)
#define scheme_eq (scheme_extension_table->scheme_eq)
#define scheme_eqv (scheme_extension_table->scheme_eqv)
#define scheme_equal (scheme_extension_table->scheme_equal)
#define scheme_build_list (scheme_extension_table->scheme_build_list)
#define scheme_list_length (scheme_extension_table->scheme_list_length)
#define scheme_proper_list_length (scheme_extension_table->scheme_proper_list_length)
#define scheme_alloc_list (scheme_extension_table->scheme_alloc_list)
#define scheme_map_1 (scheme_extension_table->scheme_map_1)
#define scheme_car (scheme_extension_table->scheme_car)
#define scheme_cdr (scheme_extension_table->scheme_cdr)
#define scheme_cadr (scheme_extension_table->scheme_cadr)
#define scheme_caddr (scheme_extension_table->scheme_caddr)
#define scheme_vector_to_list (scheme_extension_table->scheme_vector_to_list)
#define scheme_list_to_vector (scheme_extension_table->scheme_list_to_vector)
#define scheme_append (scheme_extension_table->scheme_append)
#define scheme_box (scheme_extension_table->scheme_box)
#define scheme_unbox (scheme_extension_table->scheme_unbox)
#define scheme_set_box (scheme_extension_table->scheme_set_box)
#define scheme_make_weak_box (scheme_extension_table->scheme_make_weak_box)
#define scheme_load (scheme_extension_table->scheme_load)
#define scheme_load_extension (scheme_extension_table->scheme_load_extension)
#define scheme_register_extension_global (scheme_extension_table->scheme_register_extension_global)
#define scheme_get_milliseconds (scheme_extension_table->scheme_get_milliseconds)
#define scheme_get_process_milliseconds (scheme_extension_table->scheme_get_process_milliseconds)
#define scheme_rep (scheme_extension_table->scheme_rep)
#define scheme_banner (scheme_extension_table->scheme_banner)
#define scheme_version (scheme_extension_table->scheme_version)
#define scheme_check_proc_arity (scheme_extension_table->scheme_check_proc_arity)
#define scheme_make_provided_string (scheme_extension_table->scheme_make_provided_string)
#define scheme_make_args_string (scheme_extension_table->scheme_make_args_string)
#define scheme_no_dumps (scheme_extension_table->scheme_no_dumps)
#define scheme_system_library_subpath (scheme_extension_table->scheme_system_library_subpath)

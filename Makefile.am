AUTOMAKE_OPTIONS = subdir-objects

lib_LIBRARIES = libemoskit.a
libemoskit_a_SOURCES = 4-minheap-internal.cpp		\
					   client_config.cpp 			\
					   common.cpp 					\
					   config_parser.cpp 			\
					   ev_epoll.cpp 				\
					   ev_loop.cpp 					\
					   logger.cpp 					\
					   murmur_hash.cpp 				\
					   network_event_handler.cpp	\
					   network_proto.pb.cpp 		\
					   server.cpp 					\
					   status.cpp 					\
					   stream_buf_base.cpp 			\
					   util.cpp

bin_PROGRAMS = emoskitc
emoskitc_SOURCES = codegen/client_generator.cpp		\
				   codegen/client_generator.h		\
				   codegen/client_template.cpp 		\
				   codegen/client_template.h		\
				   codegen/name_generator.cpp 		\
				   codegen/name_generator.h			\
				   codegen/proto_parser.cpp 		\
				   codegen/proto_parser.h			\
				   codegen/server_generator.cpp 	\
				   codegen/server_generator.h		\
				   codegen/server_template.cpp 		\
				   codegen/server_template.h		\
				   codegen/syntax_tree.cpp 			\
				   codegen/syntax_tree.h			\
				   codegen/util.cpp					\
				   codegen/util.h					\
				   codegen/syntax.cpp 				\
				   codegen/IDLParser.h

pkginclude_HEADERS = backend.h						\
					 4-minheap-internal.h 			\
					 array.h 						\
					 call.h 						\
					 channel.h 						\
					 circbuf.h 						\
					 client_blocking_call.h 		\
					 client_config.h 				\
					 client_context.h 				\
					 common.h 						\
					 config_parser.h 				\
					 create_stream.h 				\
					 ev_epoll.h 					\
					 ev_loop.h 						\
					 event.h 						\
					 gcc_atomic.h 					\
					 logger.h 						\
					 method.h 						\
					 method_handler.h 				\
					 mm.h 							\
					 msg_queue.h 					\
					 msg_queue_interface.h			\
					 murmur_hash.h 					\
					 network_event_handler.h 		\
					 network_proto.pb.h 			\
					 notifier.h 					\
					 object_pool.h 					\
					 output_buffer.h 				\
					 server.h 						\
					 server_context.h 				\
					 service.h 						\
					 service_method.h 				\
					 simple_circbuf.h 				\
					 simple_msg_queue.h 			\
					 status.h 						\
					 status_code.h 					\
					 stream_base.h 					\
					 stream_buf_base.h 				\
					 task.h 						\
					 tcp_stream.h 					\
					 tcp_stream_buf.h 				\
					 thread_pool.h 					\
					 util.h 						\
					 wait_strategy.h

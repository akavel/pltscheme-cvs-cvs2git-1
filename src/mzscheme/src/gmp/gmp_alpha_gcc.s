
	.set noreorder
	.set noat


	.align	3
$C36:	.t_floating 9223372036854775808.0		


	.text
	.align	3
	.globl	scheme_gmpn_count_leading_zeros
	.ent	scheme_gmpn_count_leading_zeros
scheme_gmpn_count_leading_zeros:	
	.frame $30,0,$26
	.prologue 0
	
	bis	$31,63,$0
	
	srl	$16,32,$1
	cmovne	$1,$1,$16
	cmovne	$1,31,$0
	
	srl	$16,16,$1
	subq	$0,16,$2
	cmovne	$1,$1,$16
	cmovne	$1,$2,$0
	
	srl	$16,8,$1
	subq	$0,8,$2
	cmovne	$1,$1,$16
	cmovne	$1,$2,$0
	
	srl	$16,4,$1
	subq	$0,4,$2
	cmovne	$1,$1,$16
	cmovne	$1,$2,$0
	
	srl	$16,2,$1
	subq	$0,2,$2
	cmovne	$1,$1,$16
	cmovne	$1,$2,$0
	
	srl	$16,1,$1
	subq	$0,$1,$0
	
	ret	$31,($26),1
	.end scheme_gmpn_count_leading_zeros
	
	.text
	
	.align	3
	.globl	scheme_gmpn_invert_limb
	.ent	scheme_gmpn_invert_limb
scheme_gmpn_invert_limb:
	ldgp	$29,0($27)
	.frame	$30,0,$26
	.prologue 1
	lda	$30,-16($30)
	addq	$16,$16,$1
	bne	$1,$73
	lda	$0,-1
	br	$31,$Lend
$73:
	srl	$16,1,$1
	stq	$1,0($30)
	ldt	$f11,0($30)
	cvtqt	$f11,$f1
	lda	$1,$C36
	ldt	$f10,0($1)
	divt	$f10,$f1,$f10
	lda	$2,$invtab-4096
	srl	$16,52,$1
	addq	$1,$1,$1
	addq	$1,$2,$1
	bic	$1,6,$2
	ldq	$2,0($2)
	bic	$1,1,$1
	extwl	$2,$1,$2
	sll	$2,48,$0
	umulh	$16,$0,$1
	addq	$16,$1,$3
	stq	$3,0($30)
	ldt	$f11,0($30)
	cvtqt	$f11,$f1
	mult	$f1,$f10,$f1
	cvttqc	$f1,$f1
	stt	$f1,0($30)
	ldq	$4,0($30)
	subq	$0,$4,$0
	umulh	$16,$0,$1
	mulq	$16,$0,$2
	addq	$16,$1,$3
	bge	$3,$Loop2
$Loop1:	addq	$2,$16,$2
	cmpult	$2,$16,$1
	addq	$3,$1,$3
	addq	$0,1,$0
	blt	$3,$Loop1
$Loop2:	cmpult	$2,$16,$1
	subq	$0,1,$0
	subq	$3,$1,$3
	subq	$2,$16,$2
	bge	$3,$Loop2
$Lend:
	lda	$30,16($30)
	ret	$31,($26),1

	.end	scheme_gmpn_invert_limb

	.data

$invtab:
	.word 0xffff,0xffc0,0xff80,0xff40,0xff00,0xfec0,0xfe81,0xfe41
	.word 0xfe01,0xfdc2,0xfd83,0xfd43,0xfd04,0xfcc5,0xfc86,0xfc46
	.word 0xfc07,0xfbc8,0xfb8a,0xfb4b,0xfb0c,0xfacd,0xfa8e,0xfa50
	.word 0xfa11,0xf9d3,0xf994,0xf956,0xf918,0xf8d9,0xf89b,0xf85d
	.word 0xf81f,0xf7e1,0xf7a3,0xf765,0xf727,0xf6ea,0xf6ac,0xf66e
	.word 0xf631,0xf5f3,0xf5b6,0xf578,0xf53b,0xf4fd,0xf4c0,0xf483
	.word 0xf446,0xf409,0xf3cc,0xf38f,0xf352,0xf315,0xf2d8,0xf29c
	.word 0xf25f,0xf222,0xf1e6,0xf1a9,0xf16d,0xf130,0xf0f4,0xf0b8
	.word 0xf07c,0xf03f,0xf003,0xefc7,0xef8b,0xef4f,0xef14,0xeed8
	.word 0xee9c,0xee60,0xee25,0xede9,0xedae,0xed72,0xed37,0xecfb
	.word 0xecc0,0xec85,0xec4a,0xec0e,0xebd3,0xeb98,0xeb5d,0xeb22
	.word 0xeae8,0xeaad,0xea72,0xea37,0xe9fd,0xe9c2,0xe988,0xe94d
	.word 0xe913,0xe8d8,0xe89e,0xe864,0xe829,0xe7ef,0xe7b5,0xe77b
	.word 0xe741,0xe707,0xe6cd,0xe694,0xe65a,0xe620,0xe5e6,0xe5ad
	.word 0xe573,0xe53a,0xe500,0xe4c7,0xe48d,0xe454,0xe41b,0xe3e2
	.word 0xe3a9,0xe370,0xe336,0xe2fd,0xe2c5,0xe28c,0xe253,0xe21a
	.word 0xe1e1,0xe1a9,0xe170,0xe138,0xe0ff,0xe0c7,0xe08e,0xe056
	.word 0xe01e,0xdfe5,0xdfad,0xdf75,0xdf3d,0xdf05,0xdecd,0xde95
	.word 0xde5d,0xde25,0xdded,0xddb6,0xdd7e,0xdd46,0xdd0f,0xdcd7
	.word 0xdca0,0xdc68,0xdc31,0xdbf9,0xdbc2,0xdb8b,0xdb54,0xdb1d
	.word 0xdae6,0xdaae,0xda78,0xda41,0xda0a,0xd9d3,0xd99c,0xd965
	.word 0xd92f,0xd8f8,0xd8c1,0xd88b,0xd854,0xd81e,0xd7e8,0xd7b1
	.word 0xd77b,0xd745,0xd70e,0xd6d8,0xd6a2,0xd66c,0xd636,0xd600
	.word 0xd5ca,0xd594,0xd55f,0xd529,0xd4f3,0xd4bd,0xd488,0xd452
	.word 0xd41d,0xd3e7,0xd3b2,0xd37c,0xd347,0xd312,0xd2dd,0xd2a7
	.word 0xd272,0xd23d,0xd208,0xd1d3,0xd19e,0xd169,0xd134,0xd100
	.word 0xd0cb,0xd096,0xd061,0xd02d,0xcff8,0xcfc4,0xcf8f,0xcf5b
	.word 0xcf26,0xcef2,0xcebe,0xce89,0xce55,0xce21,0xcded,0xcdb9
	.word 0xcd85,0xcd51,0xcd1d,0xcce9,0xccb5,0xcc81,0xcc4e,0xcc1a
	.word 0xcbe6,0xcbb3,0xcb7f,0xcb4c,0xcb18,0xcae5,0xcab1,0xca7e
	.word 0xca4b,0xca17,0xc9e4,0xc9b1,0xc97e,0xc94b,0xc918,0xc8e5
	.word 0xc8b2,0xc87f,0xc84c,0xc819,0xc7e7,0xc7b4,0xc781,0xc74f
	.word 0xc71c,0xc6e9,0xc6b7,0xc684,0xc652,0xc620,0xc5ed,0xc5bb
	.word 0xc589,0xc557,0xc524,0xc4f2,0xc4c0,0xc48e,0xc45c,0xc42a
	.word 0xc3f8,0xc3c7,0xc395,0xc363,0xc331,0xc300,0xc2ce,0xc29c
	.word 0xc26b,0xc239,0xc208,0xc1d6,0xc1a5,0xc174,0xc142,0xc111
	.word 0xc0e0,0xc0af,0xc07e,0xc04d,0xc01c,0xbfeb,0xbfba,0xbf89
	.word 0xbf58,0xbf27,0xbef6,0xbec5,0xbe95,0xbe64,0xbe33,0xbe03
	.word 0xbdd2,0xbda2,0xbd71,0xbd41,0xbd10,0xbce0,0xbcb0,0xbc80
	.word 0xbc4f,0xbc1f,0xbbef,0xbbbf,0xbb8f,0xbb5f,0xbb2f,0xbaff
	.word 0xbacf,0xba9f,0xba6f,0xba40,0xba10,0xb9e0,0xb9b1,0xb981
	.word 0xb951,0xb922,0xb8f2,0xb8c3,0xb894,0xb864,0xb835,0xb806
	.word 0xb7d6,0xb7a7,0xb778,0xb749,0xb71a,0xb6eb,0xb6bc,0xb68d
	.word 0xb65e,0xb62f,0xb600,0xb5d1,0xb5a2,0xb574,0xb545,0xb516
	.word 0xb4e8,0xb4b9,0xb48a,0xb45c,0xb42e,0xb3ff,0xb3d1,0xb3a2
	.word 0xb374,0xb346,0xb318,0xb2e9,0xb2bb,0xb28d,0xb25f,0xb231
	.word 0xb203,0xb1d5,0xb1a7,0xb179,0xb14b,0xb11d,0xb0f0,0xb0c2
	.word 0xb094,0xb067,0xb039,0xb00b,0xafde,0xafb0,0xaf83,0xaf55
	.word 0xaf28,0xaefb,0xaecd,0xaea0,0xae73,0xae45,0xae18,0xadeb
	.word 0xadbe,0xad91,0xad64,0xad37,0xad0a,0xacdd,0xacb0,0xac83
	.word 0xac57,0xac2a,0xabfd,0xabd0,0xaba4,0xab77,0xab4a,0xab1e
	.word 0xaaf1,0xaac5,0xaa98,0xaa6c,0xaa40,0xaa13,0xa9e7,0xa9bb
	.word 0xa98e,0xa962,0xa936,0xa90a,0xa8de,0xa8b2,0xa886,0xa85a
	.word 0xa82e,0xa802,0xa7d6,0xa7aa,0xa77e,0xa753,0xa727,0xa6fb
	.word 0xa6d0,0xa6a4,0xa678,0xa64d,0xa621,0xa5f6,0xa5ca,0xa59f
	.word 0xa574,0xa548,0xa51d,0xa4f2,0xa4c6,0xa49b,0xa470,0xa445
	.word 0xa41a,0xa3ef,0xa3c4,0xa399,0xa36e,0xa343,0xa318,0xa2ed
	.word 0xa2c2,0xa297,0xa26d,0xa242,0xa217,0xa1ed,0xa1c2,0xa197
	.word 0xa16d,0xa142,0xa118,0xa0ed,0xa0c3,0xa098,0xa06e,0xa044
	.word 0xa01a,0x9fef,0x9fc5,0x9f9b,0x9f71,0x9f47,0x9f1c,0x9ef2
	.word 0x9ec8,0x9e9e,0x9e74,0x9e4b,0x9e21,0x9df7,0x9dcd,0x9da3
	.word 0x9d79,0x9d50,0x9d26,0x9cfc,0x9cd3,0x9ca9,0x9c80,0x9c56
	.word 0x9c2d,0x9c03,0x9bda,0x9bb0,0x9b87,0x9b5e,0x9b34,0x9b0b
	.word 0x9ae2,0x9ab9,0x9a8f,0x9a66,0x9a3d,0x9a14,0x99eb,0x99c2
	.word 0x9999,0x9970,0x9947,0x991e,0x98f6,0x98cd,0x98a4,0x987b
	.word 0x9852,0x982a,0x9801,0x97d8,0x97b0,0x9787,0x975f,0x9736
	.word 0x970e,0x96e5,0x96bd,0x9695,0x966c,0x9644,0x961c,0x95f3
	.word 0x95cb,0x95a3,0x957b,0x9553,0x952b,0x9503,0x94db,0x94b3
	.word 0x948b,0x9463,0x943b,0x9413,0x93eb,0x93c3,0x939b,0x9374
	.word 0x934c,0x9324,0x92fd,0x92d5,0x92ad,0x9286,0x925e,0x9237
	.word 0x920f,0x91e8,0x91c0,0x9199,0x9172,0x914a,0x9123,0x90fc
	.word 0x90d4,0x90ad,0x9086,0x905f,0x9038,0x9011,0x8fea,0x8fc3
	.word 0x8f9c,0x8f75,0x8f4e,0x8f27,0x8f00,0x8ed9,0x8eb2,0x8e8b
	.word 0x8e65,0x8e3e,0x8e17,0x8df1,0x8dca,0x8da3,0x8d7d,0x8d56
	.word 0x8d30,0x8d09,0x8ce3,0x8cbc,0x8c96,0x8c6f,0x8c49,0x8c23
	.word 0x8bfc,0x8bd6,0x8bb0,0x8b8a,0x8b64,0x8b3d,0x8b17,0x8af1
	.word 0x8acb,0x8aa5,0x8a7f,0x8a59,0x8a33,0x8a0d,0x89e7,0x89c1
	.word 0x899c,0x8976,0x8950,0x892a,0x8904,0x88df,0x88b9,0x8893
	.word 0x886e,0x8848,0x8823,0x87fd,0x87d8,0x87b2,0x878d,0x8767
	.word 0x8742,0x871d,0x86f7,0x86d2,0x86ad,0x8687,0x8662,0x863d
	.word 0x8618,0x85f3,0x85ce,0x85a9,0x8583,0x855e,0x8539,0x8514
	.word 0x84f0,0x84cb,0x84a6,0x8481,0x845c,0x8437,0x8412,0x83ee
	.word 0x83c9,0x83a4,0x8380,0x835b,0x8336,0x8312,0x82ed,0x82c9
	.word 0x82a4,0x8280,0x825b,0x8237,0x8212,0x81ee,0x81ca,0x81a5
	.word 0x8181,0x815d,0x8138,0x8114,0x80f0,0x80cc,0x80a8,0x8084
	.word 0x8060,0x803c,0x8018,0x7ff4,0x7fd0,0x7fac,0x7f88,0x7f64
	.word 0x7f40,0x7f1c,0x7ef8,0x7ed4,0x7eb1,0x7e8d,0x7e69,0x7e45
	.word 0x7e22,0x7dfe,0x7ddb,0x7db7,0x7d93,0x7d70,0x7d4c,0x7d29
	.word 0x7d05,0x7ce2,0x7cbf,0x7c9b,0x7c78,0x7c55,0x7c31,0x7c0e
	.word 0x7beb,0x7bc7,0x7ba4,0x7b81,0x7b5e,0x7b3b,0x7b18,0x7af5
	.word 0x7ad2,0x7aaf,0x7a8c,0x7a69,0x7a46,0x7a23,0x7a00,0x79dd
	.word 0x79ba,0x7997,0x7975,0x7952,0x792f,0x790c,0x78ea,0x78c7
	.word 0x78a4,0x7882,0x785f,0x783c,0x781a,0x77f7,0x77d5,0x77b2
	.word 0x7790,0x776e,0x774b,0x7729,0x7706,0x76e4,0x76c2,0x76a0
	.word 0x767d,0x765b,0x7639,0x7617,0x75f5,0x75d2,0x75b0,0x758e
	.word 0x756c,0x754a,0x7528,0x7506,0x74e4,0x74c2,0x74a0,0x747e
	.word 0x745d,0x743b,0x7419,0x73f7,0x73d5,0x73b4,0x7392,0x7370
	.word 0x734f,0x732d,0x730b,0x72ea,0x72c8,0x72a7,0x7285,0x7264
	.word 0x7242,0x7221,0x71ff,0x71de,0x71bc,0x719b,0x717a,0x7158
	.word 0x7137,0x7116,0x70f5,0x70d3,0x70b2,0x7091,0x7070,0x704f
	.word 0x702e,0x700c,0x6feb,0x6fca,0x6fa9,0x6f88,0x6f67,0x6f46
	.word 0x6f26,0x6f05,0x6ee4,0x6ec3,0x6ea2,0x6e81,0x6e60,0x6e40
	.word 0x6e1f,0x6dfe,0x6dde,0x6dbd,0x6d9c,0x6d7c,0x6d5b,0x6d3a
	.word 0x6d1a,0x6cf9,0x6cd9,0x6cb8,0x6c98,0x6c77,0x6c57,0x6c37
	.word 0x6c16,0x6bf6,0x6bd6,0x6bb5,0x6b95,0x6b75,0x6b54,0x6b34
	.word 0x6b14,0x6af4,0x6ad4,0x6ab4,0x6a94,0x6a73,0x6a53,0x6a33
	.word 0x6a13,0x69f3,0x69d3,0x69b3,0x6993,0x6974,0x6954,0x6934
	.word 0x6914,0x68f4,0x68d4,0x68b5,0x6895,0x6875,0x6855,0x6836
	.word 0x6816,0x67f6,0x67d7,0x67b7,0x6798,0x6778,0x6758,0x6739
	.word 0x6719,0x66fa,0x66db,0x66bb,0x669c,0x667c,0x665d,0x663e
	.word 0x661e,0x65ff,0x65e0,0x65c0,0x65a1,0x6582,0x6563,0x6544
	.word 0x6524,0x6505,0x64e6,0x64c7,0x64a8,0x6489,0x646a,0x644b
	.word 0x642c,0x640d,0x63ee,0x63cf,0x63b0,0x6391,0x6373,0x6354
	.word 0x6335,0x6316,0x62f7,0x62d9,0x62ba,0x629b,0x627c,0x625e
	.word 0x623f,0x6221,0x6202,0x61e3,0x61c5,0x61a6,0x6188,0x6169
	.word 0x614b,0x612c,0x610e,0x60ef,0x60d1,0x60b3,0x6094,0x6076
	.word 0x6058,0x6039,0x601b,0x5ffd,0x5fdf,0x5fc0,0x5fa2,0x5f84
	.word 0x5f66,0x5f48,0x5f2a,0x5f0b,0x5eed,0x5ecf,0x5eb1,0x5e93
	.word 0x5e75,0x5e57,0x5e39,0x5e1b,0x5dfd,0x5de0,0x5dc2,0x5da4
	.word 0x5d86,0x5d68,0x5d4a,0x5d2d,0x5d0f,0x5cf1,0x5cd3,0x5cb6
	.word 0x5c98,0x5c7a,0x5c5d,0x5c3f,0x5c21,0x5c04,0x5be6,0x5bc9
	.word 0x5bab,0x5b8e,0x5b70,0x5b53,0x5b35,0x5b18,0x5afb,0x5add
	.word 0x5ac0,0x5aa2,0x5a85,0x5a68,0x5a4b,0x5a2d,0x5a10,0x59f3
	.word 0x59d6,0x59b8,0x599b,0x597e,0x5961,0x5944,0x5927,0x590a
	.word 0x58ed,0x58d0,0x58b3,0x5896,0x5879,0x585c,0x583f,0x5822
	.word 0x5805,0x57e8,0x57cb,0x57ae,0x5791,0x5775,0x5758,0x573b
	.word 0x571e,0x5702,0x56e5,0x56c8,0x56ac,0x568f,0x5672,0x5656
	.word 0x5639,0x561c,0x5600,0x55e3,0x55c7,0x55aa,0x558e,0x5571
	.word 0x5555,0x5538,0x551c,0x5500,0x54e3,0x54c7,0x54aa,0x548e
	.word 0x5472,0x5456,0x5439,0x541d,0x5401,0x53e5,0x53c8,0x53ac
	.word 0x5390,0x5374,0x5358,0x533c,0x5320,0x5304,0x52e8,0x52cb
	.word 0x52af,0x5293,0x5277,0x525c,0x5240,0x5224,0x5208,0x51ec
	.word 0x51d0,0x51b4,0x5198,0x517c,0x5161,0x5145,0x5129,0x510d
	.word 0x50f2,0x50d6,0x50ba,0x509f,0x5083,0x5067,0x504c,0x5030
	.word 0x5015,0x4ff9,0x4fdd,0x4fc2,0x4fa6,0x4f8b,0x4f6f,0x4f54
	.word 0x4f38,0x4f1d,0x4f02,0x4ee6,0x4ecb,0x4eb0,0x4e94,0x4e79
	.word 0x4e5e,0x4e42,0x4e27,0x4e0c,0x4df0,0x4dd5,0x4dba,0x4d9f
	.word 0x4d84,0x4d69,0x4d4d,0x4d32,0x4d17,0x4cfc,0x4ce1,0x4cc6
	.word 0x4cab,0x4c90,0x4c75,0x4c5a,0x4c3f,0x4c24,0x4c09,0x4bee
	.word 0x4bd3,0x4bb9,0x4b9e,0x4b83,0x4b68,0x4b4d,0x4b32,0x4b18
	.word 0x4afd,0x4ae2,0x4ac7,0x4aad,0x4a92,0x4a77,0x4a5d,0x4a42
	.word 0x4a27,0x4a0d,0x49f2,0x49d8,0x49bd,0x49a3,0x4988,0x496e
	.word 0x4953,0x4939,0x491e,0x4904,0x48e9,0x48cf,0x48b5,0x489a
	.word 0x4880,0x4865,0x484b,0x4831,0x4817,0x47fc,0x47e2,0x47c8
	.word 0x47ae,0x4793,0x4779,0x475f,0x4745,0x472b,0x4711,0x46f6
	.word 0x46dc,0x46c2,0x46a8,0x468e,0x4674,0x465a,0x4640,0x4626
	.word 0x460c,0x45f2,0x45d8,0x45be,0x45a5,0x458b,0x4571,0x4557
	.word 0x453d,0x4523,0x4509,0x44f0,0x44d6,0x44bc,0x44a2,0x4489
	.word 0x446f,0x4455,0x443c,0x4422,0x4408,0x43ef,0x43d5,0x43bc
	.word 0x43a2,0x4388,0x436f,0x4355,0x433c,0x4322,0x4309,0x42ef
	.word 0x42d6,0x42bc,0x42a3,0x428a,0x4270,0x4257,0x423d,0x4224
	.word 0x420b,0x41f2,0x41d8,0x41bf,0x41a6,0x418c,0x4173,0x415a
	.word 0x4141,0x4128,0x410e,0x40f5,0x40dc,0x40c3,0x40aa,0x4091
	.word 0x4078,0x405f,0x4046,0x402d,0x4014,0x3ffb,0x3fe2,0x3fc9
	.word 0x3fb0,0x3f97,0x3f7e,0x3f65,0x3f4c,0x3f33,0x3f1a,0x3f01
	.word 0x3ee8,0x3ed0,0x3eb7,0x3e9e,0x3e85,0x3e6c,0x3e54,0x3e3b
	.word 0x3e22,0x3e0a,0x3df1,0x3dd8,0x3dc0,0x3da7,0x3d8e,0x3d76
	.word 0x3d5d,0x3d45,0x3d2c,0x3d13,0x3cfb,0x3ce2,0x3cca,0x3cb1
	.word 0x3c99,0x3c80,0x3c68,0x3c50,0x3c37,0x3c1f,0x3c06,0x3bee
	.word 0x3bd6,0x3bbd,0x3ba5,0x3b8d,0x3b74,0x3b5c,0x3b44,0x3b2b
	.word 0x3b13,0x3afb,0x3ae3,0x3acb,0x3ab2,0x3a9a,0x3a82,0x3a6a
	.word 0x3a52,0x3a3a,0x3a22,0x3a09,0x39f1,0x39d9,0x39c1,0x39a9
	.word 0x3991,0x3979,0x3961,0x3949,0x3931,0x3919,0x3901,0x38ea
	.word 0x38d2,0x38ba,0x38a2,0x388a,0x3872,0x385a,0x3843,0x382b
	.word 0x3813,0x37fb,0x37e3,0x37cc,0x37b4,0x379c,0x3785,0x376d
	.word 0x3755,0x373e,0x3726,0x370e,0x36f7,0x36df,0x36c8,0x36b0
	.word 0x3698,0x3681,0x3669,0x3652,0x363a,0x3623,0x360b,0x35f4
	.word 0x35dc,0x35c5,0x35ae,0x3596,0x357f,0x3567,0x3550,0x3539
	.word 0x3521,0x350a,0x34f3,0x34db,0x34c4,0x34ad,0x3496,0x347e
	.word 0x3467,0x3450,0x3439,0x3422,0x340a,0x33f3,0x33dc,0x33c5
	.word 0x33ae,0x3397,0x3380,0x3368,0x3351,0x333a,0x3323,0x330c
	.word 0x32f5,0x32de,0x32c7,0x32b0,0x3299,0x3282,0x326c,0x3255
	.word 0x323e,0x3227,0x3210,0x31f9,0x31e2,0x31cb,0x31b5,0x319e
	.word 0x3187,0x3170,0x3159,0x3143,0x312c,0x3115,0x30fe,0x30e8
	.word 0x30d1,0x30ba,0x30a4,0x308d,0x3076,0x3060,0x3049,0x3033
	.word 0x301c,0x3005,0x2fef,0x2fd8,0x2fc2,0x2fab,0x2f95,0x2f7e
	.word 0x2f68,0x2f51,0x2f3b,0x2f24,0x2f0e,0x2ef8,0x2ee1,0x2ecb
	.word 0x2eb4,0x2e9e,0x2e88,0x2e71,0x2e5b,0x2e45,0x2e2e,0x2e18
	.word 0x2e02,0x2dec,0x2dd5,0x2dbf,0x2da9,0x2d93,0x2d7c,0x2d66
	.word 0x2d50,0x2d3a,0x2d24,0x2d0e,0x2cf8,0x2ce1,0x2ccb,0x2cb5
	.word 0x2c9f,0x2c89,0x2c73,0x2c5d,0x2c47,0x2c31,0x2c1b,0x2c05
	.word 0x2bef,0x2bd9,0x2bc3,0x2bad,0x2b97,0x2b81,0x2b6c,0x2b56
	.word 0x2b40,0x2b2a,0x2b14,0x2afe,0x2ae8,0x2ad3,0x2abd,0x2aa7
	.word 0x2a91,0x2a7c,0x2a66,0x2a50,0x2a3a,0x2a25,0x2a0f,0x29f9
	.word 0x29e4,0x29ce,0x29b8,0x29a3,0x298d,0x2977,0x2962,0x294c
	.word 0x2937,0x2921,0x290c,0x28f6,0x28e0,0x28cb,0x28b5,0x28a0
	.word 0x288b,0x2875,0x2860,0x284a,0x2835,0x281f,0x280a,0x27f5
	.word 0x27df,0x27ca,0x27b4,0x279f,0x278a,0x2774,0x275f,0x274a
	.word 0x2735,0x271f,0x270a,0x26f5,0x26e0,0x26ca,0x26b5,0x26a0
	.word 0x268b,0x2676,0x2660,0x264b,0x2636,0x2621,0x260c,0x25f7
	.word 0x25e2,0x25cd,0x25b8,0x25a2,0x258d,0x2578,0x2563,0x254e
	.word 0x2539,0x2524,0x250f,0x24fa,0x24e5,0x24d1,0x24bc,0x24a7
	.word 0x2492,0x247d,0x2468,0x2453,0x243e,0x2429,0x2415,0x2400
	.word 0x23eb,0x23d6,0x23c1,0x23ad,0x2398,0x2383,0x236e,0x235a
	.word 0x2345,0x2330,0x231c,0x2307,0x22f2,0x22dd,0x22c9,0x22b4
	.word 0x22a0,0x228b,0x2276,0x2262,0x224d,0x2239,0x2224,0x2210
	.word 0x21fb,0x21e6,0x21d2,0x21bd,0x21a9,0x2194,0x2180,0x216c
	.word 0x2157,0x2143,0x212e,0x211a,0x2105,0x20f1,0x20dd,0x20c8
	.word 0x20b4,0x20a0,0x208b,0x2077,0x2063,0x204e,0x203a,0x2026
	.word 0x2012,0x1ffd,0x1fe9,0x1fd5,0x1fc1,0x1fac,0x1f98,0x1f84
	.word 0x1f70,0x1f5c,0x1f47,0x1f33,0x1f1f,0x1f0b,0x1ef7,0x1ee3
	.word 0x1ecf,0x1ebb,0x1ea7,0x1e93,0x1e7f,0x1e6a,0x1e56,0x1e42
	.word 0x1e2e,0x1e1a,0x1e06,0x1df3,0x1ddf,0x1dcb,0x1db7,0x1da3
	.word 0x1d8f,0x1d7b,0x1d67,0x1d53,0x1d3f,0x1d2b,0x1d18,0x1d04
	.word 0x1cf0,0x1cdc,0x1cc8,0x1cb5,0x1ca1,0x1c8d,0x1c79,0x1c65
	.word 0x1c52,0x1c3e,0x1c2a,0x1c17,0x1c03,0x1bef,0x1bdb,0x1bc8
	.word 0x1bb4,0x1ba0,0x1b8d,0x1b79,0x1b66,0x1b52,0x1b3e,0x1b2b
	.word 0x1b17,0x1b04,0x1af0,0x1add,0x1ac9,0x1ab6,0x1aa2,0x1a8f
	.word 0x1a7b,0x1a68,0x1a54,0x1a41,0x1a2d,0x1a1a,0x1a06,0x19f3
	.word 0x19e0,0x19cc,0x19b9,0x19a5,0x1992,0x197f,0x196b,0x1958
	.word 0x1945,0x1931,0x191e,0x190b,0x18f8,0x18e4,0x18d1,0x18be
	.word 0x18ab,0x1897,0x1884,0x1871,0x185e,0x184b,0x1837,0x1824
	.word 0x1811,0x17fe,0x17eb,0x17d8,0x17c4,0x17b1,0x179e,0x178b
	.word 0x1778,0x1765,0x1752,0x173f,0x172c,0x1719,0x1706,0x16f3
	.word 0x16e0,0x16cd,0x16ba,0x16a7,0x1694,0x1681,0x166e,0x165b
	.word 0x1648,0x1635,0x1623,0x1610,0x15fd,0x15ea,0x15d7,0x15c4
	.word 0x15b1,0x159f,0x158c,0x1579,0x1566,0x1553,0x1541,0x152e
	.word 0x151b,0x1508,0x14f6,0x14e3,0x14d0,0x14bd,0x14ab,0x1498
	.word 0x1485,0x1473,0x1460,0x144d,0x143b,0x1428,0x1416,0x1403
	.word 0x13f0,0x13de,0x13cb,0x13b9,0x13a6,0x1394,0x1381,0x136f
	.word 0x135c,0x1349,0x1337,0x1325,0x1312,0x1300,0x12ed,0x12db
	.word 0x12c8,0x12b6,0x12a3,0x1291,0x127f,0x126c,0x125a,0x1247
	.word 0x1235,0x1223,0x1210,0x11fe,0x11ec,0x11d9,0x11c7,0x11b5
	.word 0x11a3,0x1190,0x117e,0x116c,0x1159,0x1147,0x1135,0x1123
	.word 0x1111,0x10fe,0x10ec,0x10da,0x10c8,0x10b6,0x10a4,0x1091
	.word 0x107f,0x106d,0x105b,0x1049,0x1037,0x1025,0x1013,0x1001
	.word 0x0fef,0x0fdc,0x0fca,0x0fb8,0x0fa6,0x0f94,0x0f82,0x0f70
	.word 0x0f5e,0x0f4c,0x0f3a,0x0f28,0x0f17,0x0f05,0x0ef3,0x0ee1
	.word 0x0ecf,0x0ebd,0x0eab,0x0e99,0x0e87,0x0e75,0x0e64,0x0e52
	.word 0x0e40,0x0e2e,0x0e1c,0x0e0a,0x0df9,0x0de7,0x0dd5,0x0dc3
	.word 0x0db2,0x0da0,0x0d8e,0x0d7c,0x0d6b,0x0d59,0x0d47,0x0d35
	.word 0x0d24,0x0d12,0x0d00,0x0cef,0x0cdd,0x0ccb,0x0cba,0x0ca8
	.word 0x0c97,0x0c85,0x0c73,0x0c62,0x0c50,0x0c3f,0x0c2d,0x0c1c
	.word 0x0c0a,0x0bf8,0x0be7,0x0bd5,0x0bc4,0x0bb2,0x0ba1,0x0b8f
	.word 0x0b7e,0x0b6c,0x0b5b,0x0b4a,0x0b38,0x0b27,0x0b15,0x0b04
	.word 0x0af2,0x0ae1,0x0ad0,0x0abe,0x0aad,0x0a9c,0x0a8a,0x0a79
	.word 0x0a68,0x0a56,0x0a45,0x0a34,0x0a22,0x0a11,0x0a00,0x09ee
	.word 0x09dd,0x09cc,0x09bb,0x09a9,0x0998,0x0987,0x0976,0x0965
	.word 0x0953,0x0942,0x0931,0x0920,0x090f,0x08fe,0x08ec,0x08db
	.word 0x08ca,0x08b9,0x08a8,0x0897,0x0886,0x0875,0x0864,0x0853
	.word 0x0842,0x0831,0x081f,0x080e,0x07fd,0x07ec,0x07db,0x07ca
	.word 0x07b9,0x07a8,0x0798,0x0787,0x0776,0x0765,0x0754,0x0743
	.word 0x0732,0x0721,0x0710,0x06ff,0x06ee,0x06dd,0x06cd,0x06bc
	.word 0x06ab,0x069a,0x0689,0x0678,0x0668,0x0657,0x0646,0x0635
	.word 0x0624,0x0614,0x0603,0x05f2,0x05e1,0x05d1,0x05c0,0x05af
	.word 0x059e,0x058e,0x057d,0x056c,0x055c,0x054b,0x053a,0x052a
	.word 0x0519,0x0508,0x04f8,0x04e7,0x04d6,0x04c6,0x04b5,0x04a5
	.word 0x0494,0x0484,0x0473,0x0462,0x0452,0x0441,0x0431,0x0420
	.word 0x0410,0x03ff,0x03ef,0x03de,0x03ce,0x03bd,0x03ad,0x039c
	.word 0x038c,0x037b,0x036b,0x035b,0x034a,0x033a,0x0329,0x0319
	.word 0x0309,0x02f8,0x02e8,0x02d7,0x02c7,0x02b7,0x02a6,0x0296
	.word 0x0286,0x0275,0x0265,0x0255,0x0245,0x0234,0x0224,0x0214
	.word 0x0204,0x01f3,0x01e3,0x01d3,0x01c3,0x01b2,0x01a2,0x0192
	.word 0x0182,0x0172,0x0161,0x0151,0x0141,0x0131,0x0121,0x0111
	.word 0x0101,0x00f0,0x00e0,0x00d0,0x00c0,0x00b0,0x00a0,0x0090
	.word 0x0080,0x0070,0x0060,0x0050,0x0040,0x0030,0x0020,0x0010

import os
import glob
import pandas as pd
from sqlalchemy import create_engine

# ============================================================
# CONFIGURAÇÃO
# ============================================================

SENHA_POSTGRES = "1000"

DB_URL = f"postgresql://postgres:{SENHA_POSTGRES}@localhost:5432/postgres"

# Nome da tabela no PostgreSQL
NOME_TABELA = "NAVEGAÇÃO_GLONASS"

# ============================================================
# LIGAÇÃO AO POSTGRESQL
# ============================================================

engine = create_engine(DB_URL)

pasta_atual = os.getcwd()

print(f"📁 Procurando arquivos CSV em:\n   {pasta_atual}\n")

arquivos_csv = glob.glob("*.csv")

if not arquivos_csv:
    print("❌ Nenhum arquivo .csv encontrado nesta pasta!")

else:

    print(f"✅ Encontrados {len(arquivos_csv)} arquivo(s) CSV.\n")

    # ========================================================
    # PROCESSAMENTO DOS FICHEIROS
    # ========================================================

    primeira_tabela = True

    for filepath in arquivos_csv:

        nome_arquivo = os.path.basename(filepath)

        print(f"⏳ Processando: {nome_arquivo}...")

        try:

            # ------------------------------------------------
            # 1. LER CSV
            # ------------------------------------------------
            #
            # sep=None permite ao pandas tentar detetar
            # automaticamente se o separador é , ou ;
            #

            df = pd.read_csv(
                filepath,
                sep=None,
                engine="python",
                encoding="utf-8-sig",
                on_bad_lines="skip"
            )

            print(f"   → {len(df)} linhas encontradas")
            print(f"   → {len(df.columns)} colunas encontradas")

            # ------------------------------------------------
            # 2. LIMPAR NOMES DAS COLUNAS
            # ------------------------------------------------

            df.columns = (
                df.columns
                .str.strip()
                .str.replace(" ", "_")
            )

            # ------------------------------------------------
            # 3. REMOVER COLUNAS COMPLETAMENTE VAZIAS
            # ------------------------------------------------

            df = df.dropna(axis=1, how="all")

            # ------------------------------------------------
            # 4. CONVERTER TIME PARA TIMESTAMP
            # ------------------------------------------------

            if "Time" in df.columns:

                df["Time"] = pd.to_datetime(
                    df["Time"],
                    errors="coerce",
                    dayfirst=True
                )

                # Renomear para timestamp
                df = df.rename(columns={
                    "Time": "timestamp"
                })

            else:

                print("   ⚠️ Coluna 'Time' não encontrada!")

            # ------------------------------------------------
            # 5. CONVERTER COLUNAS NUMÉRICAS
            # ------------------------------------------------

            colunas_nao_numericas = [
                "timestamp"
            ]

            for col in df.columns:

                if col not in colunas_nao_numericas:

                    # Remove espaços
                    df[col] = (
                        df[col]
                        .astype(str)
                        .str.strip()
                    )

                    # Converte para número
                    df[col] = pd.to_numeric(
                        df[col],
                        errors="coerce"
                    )

            # ------------------------------------------------
            # 6. REMOVER LINHAS SEM TIMESTAMP
            # ------------------------------------------------

            linhas_antes = len(df)

            df = df.dropna(
                subset=["timestamp"]
            )

            linhas_depois = len(df)

            if linhas_antes != linhas_depois:

                print(
                    f"   🧹 Removidas "
                    f"{linhas_antes - linhas_depois} "
                    f"linhas sem timestamp válido."
                )

            # ------------------------------------------------
            # 7. ADICIONAR NOME DO FICHEIRO
            # ------------------------------------------------

            df["arquivo_origem"] = nome_arquivo

            # ------------------------------------------------
            # 8. MOSTRAR ESTRUTURA
            # ------------------------------------------------

            print(
                f"   → Dados válidos: "
                f"{len(df)} linhas"
            )

            print(
                f"   → Colunas: "
                f"{list(df.columns)}"
            )

            # ------------------------------------------------
            # 9. GRAVAR NO POSTGRESQL
            # ------------------------------------------------

            if primeira_tabela:

                # Primeiro ficheiro:
                # cria/substitui a tabela

                df.to_sql(
                    name=NOME_TABELA,
                    con=engine,
                    if_exists="replace",
                    index=False
                )

                primeira_tabela = False

            else:

                # Restantes ficheiros:
                # adiciona à tabela

                df.to_sql(
                    name=NOME_TABELA,
                    con=engine,
                    if_exists="append",
                    index=False
                )

            print(
                f"   ✅ {nome_arquivo} "
                f"gravado com sucesso!\n"
            )

        except Exception as e:

            print(
                f"   ❌ ERRO ao processar "
                f"{nome_arquivo}:"
            )

            print(f"      {e}\n")

    # ========================================================
    # FINAL
    # ========================================================

    print("🎉 TUDO PRONTO!")
    print(
        f"Os dados foram gravados na tabela "
        f"'{NOME_TABELA}'."
    )

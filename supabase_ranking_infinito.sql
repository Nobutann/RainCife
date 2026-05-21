create table if not exists public.ranking_infinito (
    id bigserial primary key,
    nome varchar(23) not null,
    metros real not null check (metros >= 0),
    created_at timestamptz not null default now()
);

alter table public.ranking_infinito enable row level security;

drop policy if exists "ranking_infinito_select_publico" on public.ranking_infinito;
create policy "ranking_infinito_select_publico"
on public.ranking_infinito
for select
to anon
using (true);

drop policy if exists "ranking_infinito_insert_publico" on public.ranking_infinito;
create policy "ranking_infinito_insert_publico"
on public.ranking_infinito
for insert
to anon
with check (
    char_length(nome) between 1 and 23
    and metros >= 0
);

grant select, insert on public.ranking_infinito to anon;
grant usage, select on sequence public.ranking_infinito_id_seq to anon;
